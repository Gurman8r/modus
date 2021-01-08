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

		get_bus()->get_delegate<runtime_update_event>() += [](auto const & ev) {};

		subscribe<
			runtime_startup_event,
			runtime_shutdown_event,
			runtime_update_event,
			editor_dockspace_event,
			runtime_imgui_event,
			runtime_frame_end_event
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
		// main
		case runtime_startup_event	::ID: { auto const & ev{ (runtime_startup_event const &)value }; } break;
		case runtime_shutdown_event	::ID: { auto const & ev{ (runtime_shutdown_event const &)value }; } break;
		case runtime_update_event	::ID: { auto const & ev{ (runtime_update_event const &)value }; } break;
		case editor_dockspace_event	::ID: { auto const & ev{ (editor_dockspace_event const &)value }; } break;
		case runtime_imgui_event	::ID: { auto const & ev{ (runtime_imgui_event const &)value }; } break;
		case runtime_frame_end_event::ID: { auto const & ev{ (runtime_frame_end_event const &)value }; } break;

		// input
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