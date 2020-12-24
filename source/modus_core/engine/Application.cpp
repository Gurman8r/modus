#include <modus_core/engine/Application.hpp>
#include <modus_core/events/InputEvents.hpp>
#include <modus_core/events/WindowEvents.hpp>
#include <modus_core/events/EngineEvents.hpp>
#include <modus_core/events/ImGuiEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	application::application(int32 argc, char * argv[], json const & j, allocator_type alloc)
		: gui_application	{ argc, argv, j, alloc }
		, m_plugin_manager	{ this, alloc }
	{
		ML_verify(begin_singleton<application>(this));

		subscribe
		<
			load_event,
			unload_event,

			begin_step_event,
			end_step_event,

			dock_builder_event,
			main_menu_bar_event,
			imgui_event,

			char_event,
			key_event,
			mouse_button_event,
			mouse_pos_event,
			mouse_wheel_event
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
		case load_event::ID: { auto const & ev{ (load_event const &)value }; } break;
		case unload_event::ID: { auto const & ev{ (unload_event const &)value }; } break;

		case begin_step_event::ID: { auto const & ev{ (begin_step_event const &)value }; } break;
		case end_step_event::ID: { auto const & ev{ (end_step_event const &)value }; } break;

		case dock_builder_event::ID: { auto const & ev{ (dock_builder_event const &)value }; } break;
		case main_menu_bar_event::ID: { auto const & ev{ (main_menu_bar_event const &)value }; } break;
		case imgui_event::ID: { auto const & ev{ (imgui_event const &)value }; } break;

		case char_event::ID: { auto const & ev{ (char_event const &)value }; } break;
		case key_event::ID: { auto const & ev{ (key_event const &)value }; } break;
		case mouse_button_event::ID: { auto const & ev{ (mouse_button_event const &)value }; } break;
		case mouse_pos_event::ID: { auto const & ev{ (mouse_pos_event const &)value }; } break;
		case mouse_wheel_event::ID: { auto const & ev{ (mouse_wheel_event const &)value }; } break;
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