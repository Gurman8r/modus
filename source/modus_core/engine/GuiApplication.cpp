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
		, m_main_window		{ get_bus(), alloc }
		, m_main_loop		{ get_bus(), alloc }
		, m_fps_tracker		{}
	{
		ML_assert(begin_singleton<gui_application>(this));

		ML_assert(main_window::initialize());

		main_window::set_error_callback([](int32 code, cstring desc) { /* TODO */ });

		m_main_loop.set_loop_condition(&main_window::is_open, &m_main_window);
		m_main_loop.set_enter_callback([&]() { get_bus()->fire<app_enter_event>(); });
		m_main_loop.set_exit_callback([&]() { get_bus()->fire<app_exit_event>(); });
		m_main_loop.set_idle_callback([&](auto) { get_bus()->fire<app_idle_event>(); });
	}

	gui_application::~gui_application() noexcept
	{
		main_window::finalize();

		ML_assert(end_singleton<gui_application>(this));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	std::optional<fs::path> gui_application::open_file_name(ds::string const & filter) const
	{
		if (!m_main_window.is_open()) { return std::nullopt; }
		else
		{
			return platform_api::open_file_name(m_main_window.get_native_handle(), filter.c_str());
		}
	}

	std::optional<fs::path> gui_application::save_file_name(ds::string const & filter) const
	{
		if (!m_main_window.is_open()) { return std::nullopt; }
		else
		{
			return platform_api::save_file_name(m_main_window.get_native_handle(), filter.c_str());
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void gui_application::on_event(event const & value)
	{
		core_application::on_event(value);
		switch (value)
		{
		case app_enter_event::ID: {
			auto && ev{ (app_enter_event &&)value };

			// main window
			if (attr().contains("main_window")) {
				json & win_prefs{ attr("main_window") };

				// open
				ML_assert(m_main_window.open
				(
					app_name(),
					(win_prefs.contains("video_mode")
						? win_prefs["video_mode"]
						: video_mode{}),
					(win_prefs.contains("context_settings")
						? win_prefs["context_settings"]
						: context_settings{}),
					(win_prefs.contains("window_hints")
						? win_prefs["window_hints"]
						: window_hints_default))
				);

				// theme
				if (win_prefs.contains("theme")) {
					json & theme_prefs{ win_prefs["theme"] };
					if (theme_prefs.contains("path")) {
						m_main_window.load_theme(path_to(theme_prefs["path"]));
					}
				}

				// dockspace
				if (win_prefs.contains("dockspace")) {
					json & dock_prefs{ win_prefs["dockspace"] };
					m_main_window.get_dockspace()->Configure(dock_prefs);
				}
			}

		} break;

		case app_exit_event::ID: {
			auto && ev{ (app_exit_event &&)value };
		} break;

		case app_idle_event::ID: {
			auto && ev{ (app_idle_event &&)value };

			// update fps
			m_fps_tracker(m_main_loop.delta_time());

			// imgui frame
			m_main_window.do_frame([&
				, context	= m_main_window.get_imgui().get()
				, menubar	= m_main_window.get_menubar()
				, dockspace	= m_main_window.get_dockspace()
			](auto)
			{
				dockspace->SetWindowFlag(
					ImGuiWindowFlags_MenuBar,
					ImGui::FindWindowByName(menubar->Title));

				(*dockspace)(context->Viewports[0], [&](auto) noexcept
				{
					get_bus()->fire<imgui_dockspace_event>(dockspace);
				});

				get_bus()->fire<imgui_render_event>(context);
			});

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