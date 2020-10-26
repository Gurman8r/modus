#include <modus_core/runtime/Runtime.hpp>
#include <modus_core/runtime/RuntimeEvents.hpp>
#include <modus_core/graphics/RenderWindow.hpp>
#include <modus_core/embed/Python.hpp>
#include <modus_core/window/WindowEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	static runtime_context * g_runtime{};

	runtime_context * get_global_runtime() noexcept {
		return g_runtime;
	}

	runtime_context * set_global_runtime(runtime_context * value) noexcept {
		return g_runtime = value;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	runtime_context::runtime_context(runtime_api * api)
		: runtime_listener	{ api }
		, m_running			{}
		, m_loopcond		{ std::bind(&render_window::is_open, api->win) }
		, m_plugins			{ api }
		, m_imgui			{}
		, m_dockspace		{ "##MainDockspace" }
	{
		subscribe<window_key_event>();
		subscribe<window_mouse_event>();
		subscribe<window_cursor_pos_event>();

		do_startup();
	}

	runtime_context::~runtime_context() noexcept
	{
		do_shutdown();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	int32_t runtime_context::idle()
	{
		// lock
		if (m_running || !check_condition()) { return EXIT_FAILURE; }
		else { m_running = true; } ML_defer(&) { m_running = false; };
		
		// enter / exit
		get_bus()->fire<client_enter_event>(this);
		ML_defer(&) { get_bus()->fire<client_exit_event>(this); };
		if (!check_condition()) { return EXIT_FAILURE; }

		// main loop
		do { do_idle(); } while (check_condition());
		return EXIT_SUCCESS;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void runtime_context::on_event(event && value)
	{
		switch (value)
		{
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

	void runtime_context::do_startup()
	{
		// api
		auto const api{ get_api() };
		auto & io{ *api->io };

		// preferences
		ML_assert(io.prefs.contains("runtime"));
		auto & runtime_prefs{ io.prefs["runtime"] };
		bool const req_callbacks{ runtime_prefs["callbacks"] };

		// python
		PyObject_SetArenaAllocator(([&temp = PyObjectArenaAllocator{}](auto mres) noexcept {
			temp.ctx = mres;
			temp.alloc = [](auto mres, size_t s) {
				return ((pmr::memory_resource *)mres)->allocate(s);
			};
			temp.free = [](auto mres, void * p, size_t s) {
				return ((pmr::memory_resource *)mres)->deallocate(p, s);
			};
			return &temp;
		})(get_memory()->get_resource()));
		Py_SetProgramName(io.program_name.c_str());
		Py_SetPythonHome(io.content_path.c_str());
		Py_InitializeEx(1);
		ML_assert(Py_IsInitialized());

		// window
		ML_assert(io.prefs.contains("window"));
		auto & window_prefs{ io.prefs["window"] };
		ML_assert(api->win->open(
			window_prefs["title"],
			window_prefs["video"],
			window_prefs["context"],
			window_prefs["hints"]));
		api->win->set_user_pointer(api);
		if (req_callbacks)
		{
			struct helper {
				ML_NODISCARD static event_bus * get(window_handle w)
				{
					return ((runtime_api *)render_window::get_user_pointer(w))->bus;
				}
			};

			render_window::set_error_callback([
			](auto ... x) { get_global_runtime()->get_bus()->fire<window_error_event>(x...); });

			api->win->set_char_callback([
			](auto w, auto ... x) { helper::get(w)->fire<window_char_event>(x...); });
			
			api->win->set_char_mods_callback([
			](auto w, auto ... x) { helper::get(w)->fire<window_char_mods_event>(x...); });
			
			api->win->set_close_callback([
			](auto w, auto ... x) { helper::get(w)->fire<window_close_event>(x...); });
			
			api->win->set_cursor_enter_callback([
			](auto w, auto ... x) { helper::get(w)->fire<window_cursor_enter_event>(x...); });
			
			api->win->set_cursor_pos_callback([
			](auto w, auto ... x) { helper::get(w)->fire<window_cursor_pos_event>(x...); });
			
			api->win->set_content_scale_callback([
			](auto w, auto ... x) { helper::get(w)->fire<window_content_scale_event>(x...); });
			
			api->win->set_drop_callback([
			](auto w, auto ... x) { helper::get(w)->fire<window_drop_event>(x...); });
			
			api->win->set_focus_callback([
			](auto w, auto ... x) { helper::get(w)->fire<window_focus_event>(x...); });
			
			api->win->set_framebuffer_resize_callback([
			](auto w, auto ... x) { helper::get(w)->fire<window_framebuffer_resize_event>(x...); });
			
			api->win->set_iconify_callback([
			](auto w, auto ... x) { helper::get(w)->fire<window_iconify_event>(x...); });
			
			api->win->set_key_callback([
			](auto w, auto ... x) { helper::get(w)->fire<window_key_event>(x...); });
			
			api->win->set_maximize_callback([
			](auto w, auto ... x) { helper::get(w)->fire<window_maximize_event>(x...); });
			
			api->win->set_mouse_callback([
			](auto w, auto ... x) { helper::get(w)->fire<window_mouse_event>(x...); });
			
			api->win->set_position_callback([
			](auto w, auto ... x) { helper::get(w)->fire<window_position_event>(x...); });
			
			api->win->set_refresh_callback([
			](auto w, auto ... x) { helper::get(w)->fire<window_refresh_event>(x...); });
			
			api->win->set_resize_callback([
			](auto w, auto ... x) { helper::get(w)->fire<window_resize_event>(x...); });
			
			api->win->set_scroll_callback([
			](auto w, auto ... x) { helper::get(w)->fire<window_scroll_event>(x...); });
		}

		// imgui
		ML_assert(io.prefs.contains("imgui"));
		auto & imgui_prefs{ io.prefs["imgui"] };
		ImGui::SetAllocatorFunctions(
			[](size_t s, void * u) { return ((memory_manager *)u)->allocate(s); },
			[](void * p, void * u) { return ((memory_manager *)u)->deallocate(p); },
			api->mem);
		m_imgui.reset(ML_check(ImGui::CreateContext()));
		m_imgui->IO.LogFilename = "";
		m_imgui->IO.IniFilename = "";
		m_imgui->IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		m_imgui->IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		m_imgui->IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		ML_assert(ImGui_Startup(api->win, req_callbacks));
		imgui_prefs["dockspace"]["alpha"	].get_to(m_dockspace.Alpha);
		imgui_prefs["dockspace"]["border"	].get_to(m_dockspace.Border);
		imgui_prefs["dockspace"]["padding"	].get_to(m_dockspace.Padding);
		imgui_prefs["dockspace"]["rounding"	].get_to(m_dockspace.Rounding);
		imgui_prefs["dockspace"]["size"		].get_to(m_dockspace.Size);
		if (imgui_prefs.contains("style")) {
			auto & style_prefs{ imgui_prefs["style"] };
			if (style_prefs.is_string()) {
				ImGui_LoadStyle(io.path2(style_prefs));
			}
		}

		// install plugins
		if (runtime_prefs.contains("plugins"))
		{
			for (auto const & e : runtime_prefs["plugins"])
			{
				get_plugins().install(e["path"]);
			}
		}

		// execute scripts
		if (runtime_prefs.contains("scripts"))
		{
			for (auto const & e : runtime_prefs["scripts"])
			{
				Python_DoFile(io.path2(e["path"]));
			}
		}
	}

	void runtime_context::do_shutdown()
	{
		ImGui_Shutdown(get_window(), m_imgui.release());

		ML_assert(Py_FinalizeEx() == EXIT_SUCCESS);
	}

	void runtime_context::do_idle()
	{
		// timers
		auto ML_anon{ process_timers(get_io()) };

		// poll events
		native_window::poll_events();

		// update
		get_bus()->fire<client_idle_event>(this);

		// gui
		ImGui_DoFrame(get_window(), get_imgui(), [&]() noexcept { do_imgui(); });

		// swap buffers
		if (get_window()->has_hints(window_hints_doublebuffer))
		{
			native_window::swap_buffers(get_window()->get_handle());
		}
	}

	void runtime_context::do_imgui()
	{
		ML_ImGui_ScopeID(this);

		ML_flag_write(
			m_dockspace.WinFlags,
			ImGuiWindowFlags_MenuBar,
			ImGui::FindWindowByName("##MainMenuBar"));
		m_dockspace(m_imgui->Viewports[0], [&]() noexcept
		{
			get_bus()->fire<imgui_docker_event>(this);
		});

		get_bus()->fire<imgui_render_event>(this);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}