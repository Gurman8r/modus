#include <modus_core/runtime/Application.hpp>
#include <modus_core/events/InputEvents.hpp>
#include <modus_core/events/WindowEvents.hpp>
#include <modus_core/events/RuntimeEvents.hpp>
#include <modus_core/embed/Python.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	application::application(int32 argc, char * argv[], json const & argj, allocator_type alloc)
		: gui_application{ argc, argv, argj, alloc }
	{
		ML_ctor_global(application);

		subscribe<
			setup_event,
			cleanup_event,
			update_event,
			late_update_event,
			dockspace_event,
			gui_event,
			end_frame_event
		>();

		if (!is_interpreter_initialized() && !library_paths().empty())
		{
			ML_verify(initialize_interpreter(app_file_name(), library_paths(0)));
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
		// main loop
		case setup_event		::ID: { auto const & ev{ (setup_event const &)value }; } break;
		case cleanup_event		::ID: { auto const & ev{ (cleanup_event const &)value }; } break;
		case update_event		::ID: { auto const & ev{ (update_event const &)value }; } break;
		case late_update_event	::ID: { auto const & ev{ (late_update_event const &)value }; } break;
		case dockspace_event	::ID: { auto const & ev{ (dockspace_event const &)value }; } break;
		case gui_event			::ID: { auto const & ev{ (gui_event const &)value }; } break;
		case end_frame_event	::ID: { auto const & ev{ (end_frame_event const &)value }; } break;

		// inputs
		case char_event			::ID: { auto const & ev{ (char_event const &)value }; } break;
		case key_event			::ID: { auto const & ev{ (key_event const &)value }; } break;
		case mouse_button_event	::ID: { auto const & ev{ (mouse_button_event const &)value }; } break;
		case mouse_pos_event	::ID: { auto const & ev{ (mouse_pos_event const &)value }; } break;
		case mouse_wheel_event	::ID: { auto const & ev{ (mouse_wheel_event const &)value }; } break;
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