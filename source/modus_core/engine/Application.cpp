#include <modus_core/engine/Application.hpp>
#include <modus_core/events/InputEvents.hpp>
#include <modus_core/events/WindowEvents.hpp>
#include <modus_core/events/EngineEvents.hpp>
#include <modus_core/events/ImGuiEvents.hpp>

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
			dock_builder_event,
			main_menu_bar_event,
			imgui_begin_event,
			imgui_render_event,
			imgui_end_event,
			key_event,
			mouse_button_event,
			mouse_pos_event
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

		case dock_builder_event::ID: {
			auto && ev{ (dock_builder_event &&)value };
		} break;

		case main_menu_bar_event::ID: {
			auto && ev{ (main_menu_bar_event &&)value };
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

		case key_event::ID: {
			auto && ev{ (key_event &&)value };
		} break;

		case mouse_button_event::ID: {
			auto && ev{ (mouse_button_event &&)value };
		} break;

		case mouse_pos_event::ID: {
			auto && ev{ (mouse_pos_event &&)value };
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