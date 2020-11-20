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
		, m_window			{ get_bus(), alloc }
		, m_fps_tracker		{}
	{
		ML_assert(begin_singleton<gui_application>(this));

		ML_assert(m_window.get_window_context()->initialize());

		main_loop()->set_loop_condition(&main_window::is_open, &m_window);
		main_loop()->set_enter_callback([&]() { get_bus()->fire<app_enter_event>(); });
		main_loop()->set_exit_callback([&]() { get_bus()->fire<app_exit_event>(); });
		main_loop()->set_idle_callback([&](auto) { get_bus()->fire<app_idle_event>(); });
	}

	gui_application::~gui_application() noexcept
	{
		m_window.get_window_context()->finalize();

		ML_assert(end_singleton<gui_application>(this));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	std::optional<fs::path> gui_application::get_open_file_name(ds::string const & filter) const
	{
		return platform_api::get_open_file_name(m_window.get_native_handle(), filter.c_str());
	}

	std::optional<fs::path> gui_application::get_save_file_name(ds::string const & filter) const
	{
		return platform_api::get_save_file_name(m_window.get_native_handle(), filter.c_str());
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void gui_application::on_event(event const & value)
	{
		core_application::on_event(value);
		switch (value)
		{
		case app_enter_event::ID: {
			auto && ev{ (app_enter_event &&)value };

			// open window
			ML_assert(attr().contains("window"));
			json & window_prefs{ attr("window") };
			ML_assert(m_window.open(
				window_prefs["title"],
				window_prefs["video"],
				window_prefs["context"],
				window_prefs["hints"]));

			// configure imgui
			if (attr().contains("imgui")) {
				json & gui_prefs{ attr("imgui") };
				if (gui_prefs.contains("dockspace")) {
					m_window.get_dockspace()->Configure(gui_prefs["dockspace"]);
				}
				if (gui_prefs.contains("style")) {
					json & style_prefs{ gui_prefs["style"] };
					if (style_prefs.is_string()) {
						m_window.load_imgui_style(path_to(style_prefs));
					}
				}
			}

		} break;

		case app_exit_event::ID: {
			auto && ev{ (app_exit_event &&)value };
		} break;

		case app_idle_event::ID: {
			auto && ev{ (app_idle_event &&)value };

			// update fps
			m_fps_tracker(main_loop()->delta_time());

			// imgui frame
			m_window.do_imgui_frame([&
				, context	= m_window.get_imgui().get()
				, docker	= m_window.get_dockspace()
			](auto)
			{
				docker->SetWindowFlag(
					ImGuiWindowFlags_MenuBar,
					ImGui::FindWindowByName("##MainMenuBar"));

				(*docker)(context->Viewports[0], [&](auto) noexcept
				{
					get_bus()->fire<imgui_dockspace_event>(docker);
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