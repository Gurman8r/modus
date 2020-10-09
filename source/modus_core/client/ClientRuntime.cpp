#include <client/ClientRuntime.hpp>
#include <client/ClientEvents.hpp>
#include <graphics/RenderWindow.hpp>
#include <embed/Python.hpp>
#include <imgui/ImGuiContext.hpp>
#include <imgui/ImGuiEvents.hpp>
#include <window/WindowEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	client_runtime::client_runtime(client_context * context)
		: client_object	{ context }
		, m_running		{}
		, m_plugins		{ context }
	{
		subscribe<client_enter_event>();
		subscribe<client_exit_event>();
		subscribe<client_update_event>();

		subscribe<imgui_dockspace_event>();
		subscribe<imgui_menubar_event>();
		subscribe<imgui_render_event>();

		subscribe<window_key_event>();
		subscribe<window_mouse_event>();
		subscribe<window_cursor_position_event>();

		PyObject_SetArenaAllocator(std::invoke([mm = get_memory()]() noexcept
		{
			static PyObjectArenaAllocator temp
			{
				mm->get_resource(),
				[](auto mres, size_t size) noexcept
				{
					return ((pmr::memory_resource *)mres)->allocate(size);
				},
				[](auto mres, void * addr, size_t size) noexcept
				{
					return ((pmr::memory_resource *)mres)->deallocate(addr, size);
				}
			};
			return &temp;
		}));
		Py_SetProgramName(get_io()->program_name.c_str());
		Py_SetPythonHome(get_io()->content_path.c_str());
		Py_InitializeEx(1);
		ML_assert(Py_IsInitialized());
	}

	client_runtime::~client_runtime()
	{
		ML_assert(Py_FinalizeEx() == EXIT_SUCCESS);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void client_runtime::on_event(event && value)
	{
		switch (value)
		{
		// client
		case client_enter_event::ID: {
			auto && ev{ (client_enter_event &&)value };
		} break;

		case client_exit_event::ID: {
			auto && ev{ (client_exit_event &&)value };
		} break;

		case client_update_event::ID: {
			auto && ev{ (client_update_event &&)value };
		} break;


		// imgui
		case imgui_dockspace_event::ID: {

		} break;

		case imgui_menubar_event::ID: {

		} break;

		case imgui_render_event::ID: {
		} break;


		// window
		case window_key_event::ID: {
			auto && ev{ (window_key_event &&)value };
			get_io()->keyboard[ev.key] = ev.action;
		} break;

		case window_mouse_event::ID: {
			auto && ev{ (window_mouse_event &&)value };
			get_io()->mouse[ev.button] = ev.action;
		} break;

		case window_cursor_position_event::ID: {
			auto && ev{ (window_cursor_position_event &&)value };
			get_io()->cursor = { ev.x, ev.y };
		} break;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	int32_t client_runtime::process(std::function<void()> const & fn)
	{
		if (m_running) { return EXIT_FAILURE; }
		else { m_running = true; } ML_defer(&) { m_running = false; };

		get_bus()->fire<client_enter_event>(get_context());
		ML_defer(&) { get_bus()->fire<client_exit_event>(get_context()); };
		if (!get_window()->is_open()) { return EXIT_FAILURE; }
		do
		{
			auto ML_anon{ std::invoke([&io = *get_io()]() noexcept
			{
				io.loop_timer.restart();
				auto const dt{ (float_t)io.frame_time.count() };
				io.fps_accum += dt - io.fps_times[io.fps_index];
				io.fps_times[io.fps_index] = dt;
				io.fps_index = (io.fps_index + 1) % io.fps_times.size();
				io.fps_rate = (0.f < io.fps_accum) ? 1.f / (io.fps_accum / (float_t)io.fps_times.size()) : FLT_MAX;
				return ML_defer_ex(&) {
					++io.frame_count;
					io.frame_time = io.loop_timer.elapsed();
				};
			}) };

			window::poll_events();

			if (fn) { std::invoke(fn); }

			get_imgui()->do_frame();

			if (get_window()->has_hints(window_hints_doublebuffer))
			{
				window::swap_buffers(get_window()->get_handle());
			}
		}
		while (get_window()->is_open());
		return EXIT_SUCCESS;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}