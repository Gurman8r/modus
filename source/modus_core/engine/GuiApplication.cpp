#include <modus_core/engine/GuiApplication.hpp>
#include <modus_core/engine/PlatformAPI.hpp>
#include <modus_core/embed/Python.hpp>
#include <modus_core/engine/EngineEvents.hpp>
#include <modus_core/window/WindowEvents.hpp>
#include <modus_core/imgui/ImGuiEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	gui_application::gui_application(int32 argc, char * argv[], allocator_type alloc)
		: core_application	{ argc, argv, alloc }
		, event_listener	{ &m_dispatcher }
		, m_dispatcher		{ alloc }
		, m_window			{ alloc }
		, m_loop			{ &m_dispatcher, alloc }
		, m_fps_tracker		{}
		, m_input_state		{}
	{
		ML_assert(begin_singleton<gui_application>(this));

		subscribe<
			app_enter_event,
			app_exit_event,
			app_idle_event,
			imgui_dockspace_event,
			imgui_menubar_event,
			imgui_render_event,
			window_cursor_pos_event,
			window_key_event,
			window_mouse_event
		>();

		ML_assert(window_context::initialize());

		window_context::set_error_callback([](int32 code, cstring desc)
		{
			debug::failure("{0}: {1}", code, desc);
		});

		m_loop.set_loop_condition
		(
			&main_window::is_open, &m_window
		);
		
		m_loop.set_enter_callback([&]()
		{
			// main window
			if (attr().contains("window"))
			{
				// window settings
				json & j_window{ attr("window") };
				if (!j_window.contains("title")) { j_window["title"] = app_name(); }
				window_settings ws{};
				j_window.get_to(ws);

				// open window
				ML_assert(m_window.open(ws.title, ws.video, ws.context, ws.hints));

				// window callbacks
				if (j_window.contains("callbacks") && j_window["callbacks"].get<bool>())
				{
					main_window::install_callbacks(&m_window, &m_dispatcher);
				}

				// imgui
				if (attr().contains("imgui"))
				{
					json & j_imgui{ attr("imgui") };

					// init
					ML_assert(m_window.initialize_imgui
					(
						j_imgui.contains("callbacks") && j_imgui["callbacks"].get<bool>()
					));

					// guistyle
					if (j_imgui.contains("guistyle")) {
						json & j_guistyle{ j_imgui["guistyle"] };
						if (j_guistyle.contains("path")) {
							m_window.load_style(path_to(j_guistyle["path"]));
						}
					}

					// menubar
					if (j_imgui.contains("menubar")) {
						json & j_menubar{ j_imgui["menubar"] };
						m_window.get_menubar()->Configure(j_menubar);
					}

					// dockspace
					if (j_imgui.contains("dockspace")) {
						json & j_dockspace{ j_imgui["dockspace"] };
						m_window.get_dockspace()->Configure(j_dockspace);
					}
				}
			}

			// enter
			m_dispatcher.fire<app_enter_event>();
		});
		
		m_loop.set_exit_callback([&]()
		{
			// exit
			m_dispatcher.fire<app_exit_event>();
		});
		
		m_loop.set_idle_callback([&](duration const & dt)
		{
			// fps
			m_fps_tracker(dt);

			// idle
			m_dispatcher.fire<app_idle_event>(dt);

			// gui
			m_window.do_frame([&
				, imgui = m_window.get_imgui().get()
				, menubar = m_window.get_menubar()
				, dockspace = m_window.get_dockspace()
			](auto) noexcept
			{
				m_dispatcher.fire<imgui_begin_event>(imgui);

				ImGuizmo::BeginFrame();

				dockspace->SetWindowFlag(ImGuiWindowFlags_MenuBar, menubar->FindByName());

				(*dockspace)(imgui->Viewports[0], [&](auto) noexcept {
					m_dispatcher.fire<imgui_dockspace_event>(dockspace);
				});

				(*menubar)([&](auto) noexcept {
					m_dispatcher.fire<imgui_menubar_event>(menubar);
				});

				m_dispatcher.fire<imgui_render_event>(imgui);

				m_dispatcher.fire<imgui_end_event>(imgui);
			});
		});
	}

	gui_application::~gui_application() noexcept
	{
		window_context::finalize();

		unsubscribe(); // manual unsubscribe required

		ML_assert(end_singleton<gui_application>(this));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	std::optional<fs::path> gui_application::open_file_name(ds::string const & filter) const
	{
		if (!m_window.is_open()) { return std::nullopt; }
		
		return platform_api::open_file_name(m_window.get_native_handle(), filter.c_str());
	}

	std::optional<fs::path> gui_application::save_file_name(ds::string const & filter) const
	{
		if (!m_window.is_open()) { return std::nullopt; }
		
		return platform_api::save_file_name(m_window.get_native_handle(), filter.c_str());
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void gui_application::on_event(event const & value)
	{
		switch (value)
		{
		case window_cursor_pos_event::ID: {
			auto && ev{ (window_cursor_pos_event &&)value };
			m_input_state.cursor_pos = { ev.x, ev.y };
		} break;

		case window_key_event::ID: {
			auto && ev{ (window_key_event &&)value };
			m_input_state.keyboard[ev.key] = ev.action;
		} break;

		case window_mouse_event::ID: {
			auto && ev{ (window_mouse_event &&)value };
			m_input_state.keyboard[ev.button] = ev.action;
		} break;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// global gui_application
namespace ml::globals
{
	static gui_application * g_gui_application{};

	ML_impl_global(gui_application) get() noexcept
	{
		return g_gui_application;
	}

	ML_impl_global(gui_application) set(gui_application * value) noexcept
	{
		return g_gui_application = value;
	}
}