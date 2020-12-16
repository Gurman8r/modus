#include <modus_core/engine/Application.hpp>
#include <modus_core/engine/EngineEvents.hpp>
#include <modus_core/window/WindowEvents.hpp>
#include <modus_core/imgui/ImGuiEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	application::application(int32 argc, char * argv[], json const & attributes, allocator_type alloc)
		: gui_application	{ argc, argv, attributes, alloc }
		, m_plugin_manager	{ this, alloc }
		, m_interpreter		{}
		, m_active_scene	{}
	{
		ML_verify(begin_singleton<application>(this));

		subscribe<
			app_enter_event,
			app_exit_event,
			app_idle_event,
			imgui_dockspace_event,
			imgui_menubar_event,
			imgui_begin_event,
			imgui_render_event,
			imgui_end_event,
			window_cursor_pos_event,
			window_key_event,
			window_mouse_event
		>();
	}

	application::~application() noexcept
	{
		ML_verify(end_singleton<application>(this));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void application::on_event(event const & value)
	{
		gui_application::on_event(value);
		switch (value)
		{
		case app_enter_event::ID: {
			auto && ev{ (app_enter_event &&)value };
		} break;

		case app_exit_event::ID: {
			auto && ev{ (app_exit_event &&)value };
		} break;

		case app_idle_event::ID: {
			auto && ev{ (app_idle_event &&)value };
		} break;

		case imgui_dockspace_event::ID: {
			auto && ev{ (imgui_dockspace_event &&)value };
		} break;

		case imgui_menubar_event::ID: {
			auto && ev{ (imgui_menubar_event &&)value };
		} break;

		case imgui_begin_event::ID: {
			auto && ev{ (imgui_begin_event &&)value };
		} break;

		case imgui_render_event::ID: {
			auto && ev{ (imgui_render_event &&)value };
		} break;

		case imgui_end_event::ID: {
			auto && ev{ (imgui_end_event &&)value };
		} break;

		case window_cursor_pos_event::ID: {
			auto && ev{ (window_cursor_pos_event &&)value };
		} break;

		case window_key_event::ID: {
			auto && ev{ (window_key_event &&)value };
		} break;

		case window_mouse_event::ID: {
			auto && ev{ (window_mouse_event &&)value };
		} break;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// global application
namespace ml::globals
{
	static application * g_application{};

	ML_impl_global(application) get() noexcept
	{
		return g_application;
	}

	ML_impl_global(application) set(application * value) noexcept
	{
		return g_application = value;
	}
}