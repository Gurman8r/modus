#include <modus_core/client/ClientRuntime.hpp>
#include <modus_core/client/ClientEvents.hpp>
#include <modus_core/graphics/RenderWindow.hpp>
#include <modus_core/embed/Python.hpp>
#include <modus_core/imgui/ImGuiRuntime.hpp>
#include <modus_core/imgui/ImGuiEvents.hpp>
#include <modus_core/window/WindowEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	client_runtime::client_runtime(client_context * context)
		: client_object	{ context }
		, m_plugins		{ context }
		, m_running		{}
	{
		// events
		subscribe<client_init_event>();
		subscribe<client_exit_event>();
		subscribe<client_idle_event>();
		subscribe<imgui_dockspace_event>();
		subscribe<imgui_menubar_event>();
		subscribe<imgui_render_event>();
		subscribe<window_key_event>();
		subscribe<window_mouse_event>();
		subscribe<window_cursor_pos_event>();

		// python
		PyObject_SetArenaAllocator(std::invoke([mres = get_memory()->get_resource()]()
		{
			static PyObjectArenaAllocator temp{
				mres,
				[](auto mres, size_t size) noexcept {
					return ((pmr::memory_resource *)mres)->allocate(size);
				},
				[](auto mres, void * addr, size_t size) noexcept {
					return ((pmr::memory_resource *)mres)->deallocate(addr, size);
				}
			}; return &temp;
		}));
		Py_SetProgramName(get_io()->program_name.c_str());
		Py_SetPythonHome(get_io()->content_path.c_str());
		Py_InitializeEx(1);
		ML_assert(Py_IsInitialized());

		// window
		if (auto const win{ get_window() }; win->open(get_io()->prefs["window"]))
		{
			static event_bus * bus{}; bus = get_bus();
			win->set_char_callback([](auto, auto ... x) { bus->fire<window_char_event>(x...); });
			win->set_char_mods_callback([](auto, auto ... x) { bus->fire<window_char_mods_event>(x...); });
			win->set_close_callback([](auto, auto ... x) { bus->fire<window_close_event>(x...); });
			win->set_cursor_enter_callback([](auto, auto ... x) { bus->fire<window_cursor_enter_event>(x...); });
			win->set_cursor_pos_callback([](auto, auto ... x) { bus->fire<window_cursor_pos_event>(x...); });
			win->set_content_scale_callback([](auto, auto ... x) { bus->fire<window_content_scale_event>(x...); });
			win->set_drop_callback([](auto, auto ... x) { bus->fire<window_drop_event>(x...); });
			win->set_error_callback([](auto ... x) { bus->fire<window_error_event>(x...); });
			win->set_focus_callback([](auto, auto ... x) { bus->fire<window_focus_event>(x...); });
			win->set_framebuffer_resize_callback([](auto, auto ... x) { bus->fire<window_framebuffer_resize_event>(x...); });
			win->set_iconify_callback([](auto, auto ... x) { bus->fire<window_iconify_event>(x...); });
			win->set_key_callback([](auto, auto ... x) { bus->fire<window_key_event>(x...); });
			win->set_maximize_callback([](auto, auto ... x) { bus->fire<window_maximize_event>(x...); });
			win->set_mouse_callback([](auto, auto ... x) { bus->fire<window_mouse_event>(x...); });
			win->set_position_callback([](auto, auto ... x) { bus->fire<window_position_event>(x...); });
			win->set_refresh_callback([](auto, auto ... x) { bus->fire<window_refresh_event>(x...); });
			win->set_resize_callback([](auto, auto ... x) { bus->fire<window_resize_event>(x...); });
			win->set_scroll_callback([](auto, auto ... x) { bus->fire<window_scroll_event>(x...); });
		}

		// imgui
		ML_assert(get_gui()->startup(get_io()->prefs["imgui"]));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	client_runtime::~client_runtime()
	{
		ML_assert(Py_FinalizeEx() == EXIT_SUCCESS);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	int32_t client_runtime::idle()
	{
		if (m_running || !get_window()->is_open()) { return EXIT_FAILURE; }
		else { m_running = true; } ML_defer(&) { m_running = false; };
		
		get_bus()->fire<client_init_event>(get_context());
		ML_defer(&) { get_bus()->fire<client_exit_event>(get_context()); };
		do
		{
			// update timers
			auto ML_anon{ std::invoke([&io = *get_io()]() noexcept {
				io.loop_timer.restart();
				auto const dt{ (float_t)io.delta_time.count() };
				io.fps_accum += dt - io.fps_times[io.fps_index];
				io.fps_times[io.fps_index] = dt;
				io.fps_index = (io.fps_index + 1) % io.fps_times.size();
				io.fps_rate = (0.f < io.fps_accum) ? 1.f / (io.fps_accum / (float_t)io.fps_times.size()) : FLT_MAX;
				return ML_defer_ex(&) {
					++io.frame_count;
					io.delta_time = io.loop_timer.elapsed();
				};
			}) };

			window::poll_events();

			get_bus()->fire<client_idle_event>(get_context());

			get_gui()->do_frame();

			if (get_window()->has_hints(window_hints_doublebuffer))
			{
				window::swap_buffers(get_window()->get_handle());
			}
		}
		while (get_window()->is_open());
		return EXIT_SUCCESS;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void client_runtime::on_event(event && value)
	{
		switch (value)
		{
		// client
		case client_init_event::ID: {
			auto && ev{ (client_init_event &&)value };
		} break;

		case client_exit_event::ID: {
			auto && ev{ (client_exit_event &&)value };
		} break;

		case client_idle_event::ID: {
			auto && ev{ (client_idle_event &&)value };
		} break;


		// imgui
		case imgui_dockspace_event::ID: {
			auto && ev{ (imgui_dockspace_event &&)value };
		} break;

		case imgui_menubar_event::ID: {
			auto && ev{ (imgui_menubar_event &&)value };
		} break;

		case imgui_render_event::ID: {
			auto && ev{ (imgui_render_event &&)value };
		} break;


		// input
		case window_key_event::ID: {
			auto && ev{ (window_key_event &&)value };
			get_io()->keyboard[ev.key] = ev.action;
		} break;

		case window_mouse_event::ID: {
			auto && ev{ (window_mouse_event &&)value };
			get_io()->mouse[ev.button] = ev.action;
		} break;

		case window_cursor_pos_event::ID: {
			auto && ev{ (window_cursor_pos_event &&)value };
			get_io()->cursor = { ev.x, ev.y };
		} break;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}