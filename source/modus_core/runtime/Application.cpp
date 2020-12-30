#include <modus_core/runtime/Application.hpp>
#include <modus_core/events/InputEvents.hpp>
#include <modus_core/events/WindowEvents.hpp>
#include <modus_core/events/RuntimeEvents.hpp>
#include <modus_core/events/ImGuiEvents.hpp>
#include <modus_core/embed/Python.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	application::application(int32 argc, char * argv[], json const & argj, allocator_type alloc)
		: gui_application{ argc, argv, argj, alloc }
	{
		ML_verify(ML_begin_global(application, this));

		subscribe<
			load_event, unload_event,

			begin_frame_event, end_frame_event,

			dock_builder_event, main_menu_bar_event, imgui_event,

			char_event, key_event, mouse_button_event, mouse_pos_event, mouse_wheel_event
		>();

		if (!is_interpreter_initialized() && !library_paths().empty())
		{
			ML_verify(initialize_interpreter(app_file_name(), library_paths(0)));
		}
	}

	application::~application() noexcept
	{
		ML_verify(ML_end_global(application, this));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void application::on_event(event const & value)
	{
		gui_application::on_event(value);
		switch (value)
		{
		case load_event::ID: {
			auto const & ev{ (load_event const &)value };
		} break;

		case unload_event::ID: {
			auto const & ev{ (unload_event const &)value };
		} break;

		case begin_frame_event::ID: {
			auto const & ev{ (begin_frame_event const &)value };
		} break;

		case end_frame_event::ID: {
			auto const & ev{ (end_frame_event const &)value };
		} break;

		case dock_builder_event::ID: {
			auto const & ev{ (dock_builder_event const &)value };
		} break;

		case main_menu_bar_event::ID: {
			auto const & ev{ (main_menu_bar_event const &)value };
		} break;

		case imgui_event::ID: {
			auto const & ev{ (imgui_event const &)value };
		} break;

		case char_event::ID: {
			auto const & ev{ (char_event const &)value };
		} break;

		case key_event::ID: {
			auto const & ev{ (key_event const &)value };
		} break;

		case mouse_button_event::ID: {
			auto const & ev{ (mouse_button_event const &)value };
		} break;

		case mouse_pos_event::ID: {
			auto const & ev{ (mouse_pos_event const &)value };
		} break;

		case mouse_wheel_event::ID: {
			auto const & ev{ (mouse_wheel_event const &)value };
		} break;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// global application
namespace ml::globals
{
	static application * g_application{};

	ML_impl_global(application) get_global() noexcept
	{
		return g_application;
	}

	ML_impl_global(application) set_global(application * value) noexcept
	{
		return g_application = value;
	}
}