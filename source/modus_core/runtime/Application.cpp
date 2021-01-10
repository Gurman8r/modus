#include <modus_core/runtime/Application.hpp>
#include <modus_core/embed/Python.hpp>
#include <modus_core/events/InputEvents.hpp>
#include <modus_core/events/WindowEvents.hpp>
#include <modus_core/events/RuntimeEvents.hpp>
#include <modus_core/events/EditorEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	application::application(int32 argc, char * argv[], json const & argj, allocator_type alloc)
		: gui_application{ argc, argv, argj, alloc }
	{
		ML_ctor_global(application);

		get_bus()->get_delegate<runtime_idle_event>() += [](auto const & ev) {};

		subscribe<
			runtime_startup_event,
			runtime_shutdown_event,
			runtime_idle_event,
			dockspace_builder_event,
			runtime_gui_event,
			runtime_end_frame_event
		>();

		if (!is_interpreter_initialized() && !get_library_paths().empty())
		{
			ML_verify(initialize_interpreter(get_app_file_name(), get_library_paths(0)));
		}
	}

	application::~application() noexcept
	{
		ML_dtor_global(application);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void application::on_event(event const & value)
	{
		gui_application::on_event(value);
		switch (value)
		{
		case runtime_startup_event	::ID: { auto const & ev{ (runtime_startup_event const &)value }; } break;
		case runtime_shutdown_event	::ID: { auto const & ev{ (runtime_shutdown_event const &)value }; } break;
		case runtime_idle_event		::ID: { auto const & ev{ (runtime_idle_event const &)value }; } break;
		case dockspace_builder_event::ID: { auto const & ev{ (dockspace_builder_event const &)value }; } break;
		case runtime_gui_event		::ID: { auto const & ev{ (runtime_gui_event const &)value }; } break;
		case runtime_end_frame_event::ID: { auto const & ev{ (runtime_end_frame_event const &)value }; } break;

		case char_event				::ID: { auto const & ev{ (char_event const &)value }; } break;
		case key_event				::ID: { auto const & ev{ (key_event const &)value }; } break;
		case mouse_button_event		::ID: { auto const & ev{ (mouse_button_event const &)value }; } break;
		case mouse_pos_event		::ID: { auto const & ev{ (mouse_pos_event const &)value }; } break;
		case mouse_wheel_event		::ID: { auto const & ev{ (mouse_wheel_event const &)value }; } break;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// global application
namespace ml::globals
{
	static application * g_application{};

	ML_impl_global(application) get_global() { return g_application; }

	ML_impl_global(application) set_global(application * value) { return g_application = value; }
}