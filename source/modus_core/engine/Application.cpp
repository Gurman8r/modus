#include <modus_core/engine/Application.hpp>
#include <modus_core/embed/Python.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	application::application(int32_t argc, char * argv[], allocator_type alloc)
		: gui_application{ argc, argv, alloc }
		, m_plugins{ this, alloc }
	{
		ML_assert(begin_singleton<application>(this));

		subscribe<imgui_dockspace_event, imgui_render_event>();

		auto const mainloop{ get_main_loop() };
		mainloop->set_loop_condition(&main_window::is_open, get_main_window());
		mainloop->set_enter_callback([&]() { get_bus()->fire<app_enter_event>(); });
		mainloop->set_exit_callback([&]() { get_bus()->fire<app_exit_event>(); });
		mainloop->set_idle_callback([&]() { get_bus()->fire<app_idle_event>(); });
	}

	application::~application() noexcept
	{
		ML_assert(end_singleton<application>(this));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void application::on_event(event const & value)
	{
		gui_application::on_event(value);
		switch (value)
		{
		case app_enter_event::ID: {
			auto && ev{ (app_enter_event &&)value };

			// scripts
			ML_assert(initialize_interpreter());
			if (attr().contains("scripts")) {
				json & script_prefs{ attr("scripts") };
				for (json const & e : script_prefs) {
					auto const path{ path_to(e["path"]).string() };
					PyRun_AnyFileEx(std::fopen(path.c_str(), "r"), path.c_str(), true);
				}
			}

		} break;

		case app_exit_event::ID: {
			auto && ev{ (app_exit_event &&)value };
			debug::ok("goodbye!");
		} break;

		case app_idle_event::ID: {
			auto && ev{ (app_idle_event &&)value };
		} break;

		case imgui_dockspace_event::ID: {
			auto && ev{ (imgui_dockspace_event &&)value };
		} break;

		case imgui_render_event::ID: {
			auto && ev{ (imgui_render_event &&)value };
		} break;

		case window_key_event::ID: {
			auto && ev{ (window_key_event &&)value };
		} break;

		case window_mouse_event::ID: {
			auto && ev{ (window_mouse_event &&)value };
		} break;

		case window_cursor_pos_event::ID: {
			auto && ev{ (window_cursor_pos_event &&)value };
		} break;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// global application
namespace ml::globals
{
	static application * g_app{};

	ML_impl_global(application) get() noexcept
	{
		return g_app;
	}

	ML_impl_global(application) set(application * value) noexcept
	{
		return g_app = value;
	}
}