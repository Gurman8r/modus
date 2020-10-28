#include <modus_core/runtime/BuiltinRuntime.hpp>
#include <modus_core/embed/Python.hpp>
#include <modus_core/graphics/RenderWindow.hpp>
#include <modus_core/imgui/ImGuiEvents.hpp>
#include <modus_core/runtime/RuntimeEvents.hpp>
#include <modus_core/window/WindowEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	builtin_runtime::builtin_runtime(runtime_api * api) noexcept
		: runtime_context	{ api }
		, m_imgui			{}
		, m_dock			{ "##MainDockspace" }
	{
		ML_assert(this == get_global_runtime());

		initialize(api);

		set_loop_condition(&render_window::is_open, get_window());
	}

	builtin_runtime::~builtin_runtime() noexcept
	{
		finalize();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void builtin_runtime::initialize(runtime_api * api)
	{
		// events
		subscribe<window_key_event>();
		subscribe<window_mouse_event>();
		subscribe<window_cursor_pos_event>();

		// preferences
		ML_assert(api->io->prefs.contains("runtime"));
		auto & runtime_prefs{ api->io->prefs["runtime"] };
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
		})(api->mem->get_resource()));
		Py_SetProgramName(api->io->program_name.c_str());
		Py_SetPythonHome(api->io->content_path.c_str());
		Py_InitializeEx(1);
		ML_assert(Py_IsInitialized());

		// window
		ML_assert(api->io->prefs.contains("window"));
		auto & window_prefs{ api->io->prefs["window"] };
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
		runtime_prefs["dockspace"]["alpha"	].get_to(m_dock.Alpha);
		runtime_prefs["dockspace"]["border"	].get_to(m_dock.Border);
		runtime_prefs["dockspace"]["padding"	].get_to(m_dock.Padding);
		runtime_prefs["dockspace"]["rounding"	].get_to(m_dock.Rounding);
		runtime_prefs["dockspace"]["size"		].get_to(m_dock.Size);
		if (runtime_prefs.contains("guistyle"))
		{
			auto & guistyle{ runtime_prefs["guistyle"] };
			if (guistyle.is_string())
			{
				ImGui_LoadStyle(api->io->path2(guistyle));
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
				Python_DoFile(api->io->path2(e["path"]));
			}
		}
	}

	void builtin_runtime::finalize()
	{
		ImGui_Shutdown(get_window(), m_imgui.release());

		ML_assert(Py_FinalizeEx() == EXIT_SUCCESS);
	}

	void builtin_runtime::on_enter() noexcept
	{
		get_bus()->fire<runtime_enter_event>(this);
	}

	void builtin_runtime::on_exit() noexcept
	{
		get_bus()->fire<runtime_exit_event>(this);
	}

	void builtin_runtime::on_idle()
	{
		// poll events
		render_window::poll_events();

		// update
		get_bus()->fire<runtime_idle_event>(this);

		// imgui
		ImGui_DoFrame(get_window(), get_imgui(), [&]() noexcept { do_imgui(); });

		// swap buffers
		if (get_window()->has_hints(window_hints_doublebuffer))
		{
			render_window::swap_buffers(get_window()->get_handle());
		}
	}

	void builtin_runtime::do_imgui()
	{
		ML_ImGui_ScopeID(this);

		// dockspace
		ML_flag_write(m_dock.WinFlags,
			ImGuiWindowFlags_MenuBar,
			ImGui::FindWindowByName("##MainMenuBar"));
		m_dock(m_imgui->Viewports[0], [&]() noexcept
		{
			get_bus()->fire<imgui_docker_event>(&m_dock);
		});

		// render
		get_bus()->fire<imgui_render_event>(m_imgui.get());
	}

	void builtin_runtime::on_event(event && value)
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

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}