#include <modus_core/client/ClientRuntime.hpp>
#include <modus_core/client/ClientEvents.hpp>
#include <modus_core/client/ClientDatabase.hpp>
#include <modus_core/graphics/RenderWindow.hpp>
#include <modus_core/embed/Python.hpp>
#include <modus_core/window/WindowEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	static client_runtime * g_client{};

	client_runtime * get_default_runtime() noexcept {
		return g_client;
	}

	client_runtime * set_default_runtime(client_runtime * value) noexcept {
		return g_client = value;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	client_runtime::client_runtime(client_context * ctx)
		: client_object	{ ctx }
		, m_idling		{}
		, m_loopcond	{ std::bind(&render_window::is_open, ctx->win) }
		, m_plugins		{ ctx }
		, m_imgui		{}
		, m_dockspace	{ "##MainDockspace" }
	{
		do_startup(ctx);
	}

	client_runtime::~client_runtime() noexcept
	{
		do_shutdown();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	int32_t client_runtime::idle()
	{
		// lock
		if (m_idling || !check_loop_condition()) { return EXIT_FAILURE; }
		else { m_idling = true; } ML_defer(&) { m_idling = false; };
		
		// enter / exit
		get_bus()->fire<client_enter_event>(this);
		ML_defer(&) { get_bus()->fire<client_exit_event>(this); };
		
		// do idle
		if (!check_loop_condition()) { return EXIT_FAILURE; }
		do
		{
			auto ML_anon{ do_update_timers(*get_io()) };

			native_window::poll_events();

			get_bus()->fire<client_idle_event>(this);

			ImGui_DoFrame(get_window(), get_imgui(), [&]() noexcept
			{
				do_gui();
			});

			if (get_window()->has_hints(window_hints_doublebuffer))
			{
				native_window::swap_buffers(get_window()->get_handle());
			}
		}
		while (check_loop_condition());
		return EXIT_SUCCESS;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void client_runtime::do_startup(client_context * ctx)
	{
		// preferences
		auto & io{ *ctx->io };
		ML_assert(io.prefs.contains("client"));
		auto & client_prefs	{ io.prefs["client"] };
		bool const should_install_callbacks{ client_prefs["callbacks"] };

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
		ML_assert(ctx->win->open(
			window_prefs["title"],
			window_prefs["video"],
			window_prefs["context"],
			window_prefs["hints"]));
		ctx->win->set_user_pointer(ctx);
		if (should_install_callbacks)
		{
			static struct {
				ML_NODISCARD event_bus * get(window_handle w) const
				{
					return ((client_context *)render_window::get_user_pointer(w))->bus;
				}
			} helper;

			render_window::set_error_callback([
			](auto ... x) { get_default_runtime()->get_bus()->fire<window_error_event>(x...); });

			ctx->win->set_char_callback([
			](auto w, auto ... x) { helper.get(w)->fire<window_char_event>(x...); });
			
			ctx->win->set_char_mods_callback([
			](auto w, auto ... x) { helper.get(w)->fire<window_char_mods_event>(x...); });
			
			ctx->win->set_close_callback([
			](auto w, auto ... x) { helper.get(w)->fire<window_close_event>(x...); });
			
			ctx->win->set_cursor_enter_callback([
			](auto w, auto ... x) { helper.get(w)->fire<window_cursor_enter_event>(x...); });
			
			ctx->win->set_cursor_pos_callback([
			](auto w, auto ... x) { helper.get(w)->fire<window_cursor_pos_event>(x...); });
			
			ctx->win->set_content_scale_callback([
			](auto w, auto ... x) { helper.get(w)->fire<window_content_scale_event>(x...); });
			
			ctx->win->set_drop_callback([
			](auto w, auto ... x) { helper.get(w)->fire<window_drop_event>(x...); });
			
			ctx->win->set_focus_callback([
			](auto w, auto ... x) { helper.get(w)->fire<window_focus_event>(x...); });
			
			ctx->win->set_framebuffer_resize_callback([
			](auto w, auto ... x) { helper.get(w)->fire<window_framebuffer_resize_event>(x...); });
			
			ctx->win->set_iconify_callback([
			](auto w, auto ... x) { helper.get(w)->fire<window_iconify_event>(x...); });
			
			ctx->win->set_key_callback([
			](auto w, auto ... x) { helper.get(w)->fire<window_key_event>(x...); });
			
			ctx->win->set_maximize_callback([
			](auto w, auto ... x) { helper.get(w)->fire<window_maximize_event>(x...); });
			
			ctx->win->set_mouse_callback([
			](auto w, auto ... x) { helper.get(w)->fire<window_mouse_event>(x...); });
			
			ctx->win->set_position_callback([
			](auto w, auto ... x) { helper.get(w)->fire<window_position_event>(x...); });
			
			ctx->win->set_refresh_callback([
			](auto w, auto ... x) { helper.get(w)->fire<window_refresh_event>(x...); });
			
			ctx->win->set_resize_callback([
			](auto w, auto ... x) { helper.get(w)->fire<window_resize_event>(x...); });
			
			ctx->win->set_scroll_callback([
			](auto w, auto ... x) { helper.get(w)->fire<window_scroll_event>(x...); });
		}

		// imgui
		ML_assert(io.prefs.contains("imgui"));
		auto & imgui_prefs{ io.prefs["imgui"] };
		ImGui::SetAllocatorFunctions(
			[](size_t s, void * u) { return ((memory_manager *)u)->allocate(s); },
			[](void * p, void * u) { return ((memory_manager *)u)->deallocate(p); },
			ctx->mem);
		m_imgui.reset(ML_check(ImGui::CreateContext()));
		m_imgui->IO.LogFilename = "";
		m_imgui->IO.IniFilename = "";
		m_imgui->IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		m_imgui->IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		m_imgui->IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		ML_assert(ImGui_Startup(ctx->win, should_install_callbacks));
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
		if (client_prefs.contains("plugins"))
		{
			for (auto const & e : client_prefs["plugins"])
			{
				m_plugins.install(e["path"]);
			}
		}

		// execute scripts
		if (client_prefs.contains("scripts"))
		{
			for (auto const & e : client_prefs["scripts"])
			{
				Python_DoFile(io.path2(e["path"]));
			}
		}
	}

	void client_runtime::do_shutdown()
	{
		ImGui_Shutdown(get_window(), m_imgui.release());

		ML_assert(Py_FinalizeEx() == EXIT_SUCCESS);
	}

	void client_runtime::do_gui()
	{
		ML_ImGui_ScopeID(this);

		ML_flag_write(m_dockspace.WinFlags, ImGuiWindowFlags_MenuBar, ImGui::FindWindowByName("##MainMenuBar"));
		m_dockspace(m_imgui->Viewports[0], [&]() noexcept
		{
			get_bus()->fire<imgui_docker_event>(this);
		});

		get_bus()->fire<imgui_render_event>(this);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}