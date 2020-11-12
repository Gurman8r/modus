#include <modus_core/engine/GuiApplication.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	gui_application::gui_application(int32_t argc, char * argv[], allocator_type alloc)
		: core_application	{ argc, argv, alloc }
		, m_loop_timer		{ false }
		, m_delta_time		{}
		, m_frame_count		{}
		, m_frame_rate		{}
		, m_fps_accum		{}
		, m_fps_index		{}
		, m_fps_times		{}
		, m_cursor_pos		{}
		, m_mouse			{}
		, m_keyboard		{}
		, m_window			{ new main_window{ get_bus(), alloc } }
	{
		ML_assert(begin_global<gui_application>(this));

		subscribe<window_cursor_pos_event, window_key_event, window_mouse_event>();
	}

	gui_application::~gui_application() noexcept
	{
		ML_assert(end_global<gui_application>(this));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void gui_application::on_event(event const & value)
	{
		core_application::on_event(ML_forward(value));

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

		case window_key_event::ID: {
			auto && ev{ (window_key_event &&)value };
			m_keyboard[ev.key] = ev.action;
		} break;

		case window_mouse_event::ID: {
			auto && ev{ (window_mouse_event &&)value };
			m_mouse[ev.button] = ev.action;
		} break;

		case window_cursor_pos_event::ID: {
			auto && ev{ (window_cursor_pos_event &&)value };
			m_cursor_pos = { ev.x, ev.y };
		} break;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// global gui_application
namespace ml::globals
{
	static gui_application * g_gui_app{};

	ML_impl_global(gui_application) get() noexcept
	{
		return g_gui_app;
	}

	ML_impl_global(gui_application) set(gui_application * value) noexcept
	{
		return g_gui_app = value;
	}
}