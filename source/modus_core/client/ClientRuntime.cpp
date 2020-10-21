#include <modus_core/client/ClientRuntime.hpp>
#include <modus_core/client/ClientEvents.hpp>
#include <modus_core/client/ClientDatabase.hpp>
#include <modus_core/client/ImGuiExt.hpp>
#include <modus_core/graphics/RenderWindow.hpp>
#include <modus_core/embed/Python.hpp>
#include <modus_core/window/WindowEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	client_runtime::client_runtime(client_context * ctx)
		: client_object		{ ctx }
		, m_idling			{}
		, m_loopcond		{ std::bind(&render_window::is_open, ctx->win) }
		, m_imgui			{}
		, m_plugins			{ ctx }
		, m_menu_enabled	{}
		, m_dock_enabled	{}
		, m_dock_title		{ ctx->mem->get_allocator() }
		, m_dock_border		{}
		, m_dock_rounding	{}
		, m_dock_alpha		{}
		, m_dock_padding	{}
		, m_dock_size		{}
		, m_dock_flags		{ ImGuiDockNodeFlags_AutoHideTabBar }
	{
		subscribe<window_key_event>();
		subscribe<window_mouse_event>();
		subscribe<window_cursor_pos_event>();

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
		do { do_idle(); } while (check_loop_condition());
		return EXIT_SUCCESS;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void client_runtime::do_startup(client_context * ctx)
	{
		// preferences
		ML_assert(ctx->io->prefs.contains("window"));
		ML_assert(ctx->io->prefs.contains("client"));
		auto & window_prefs{ ctx->io->prefs["window"] };
		auto & client_prefs{ ctx->io->prefs["client"] };

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
		})(ctx->mem->get_resource()));
		Py_SetProgramName(ctx->io->program_name.c_str());
		Py_SetPythonHome(ctx->io->content_path.c_str());
		Py_InitializeEx(1);
		ML_assert(Py_IsInitialized());

		// window
		ML_assert(ctx->win->open(
			window_prefs["title"],
			window_prefs["video"],
			window_prefs["context"],
			window_prefs["hints"]));
		if (client_prefs["callbacks"])
		{
			static event_bus * bus{}; bus = ctx->bus;
			ctx->win->set_char_callback([](auto, auto ... x) { bus->fire<window_char_event>(x...); });
			ctx->win->set_char_mods_callback([](auto, auto ... x) { bus->fire<window_char_mods_event>(x...); });
			ctx->win->set_close_callback([](auto, auto ... x) { bus->fire<window_close_event>(x...); });
			ctx->win->set_cursor_enter_callback([](auto, auto ... x) { bus->fire<window_cursor_enter_event>(x...); });
			ctx->win->set_cursor_pos_callback([](auto, auto ... x) { bus->fire<window_cursor_pos_event>(x...); });
			ctx->win->set_content_scale_callback([](auto, auto ... x) { bus->fire<window_content_scale_event>(x...); });
			ctx->win->set_drop_callback([](auto, auto ... x) { bus->fire<window_drop_event>(x...); });
			ctx->win->set_error_callback([](auto ... x) { bus->fire<window_error_event>(x...); });
			ctx->win->set_focus_callback([](auto, auto ... x) { bus->fire<window_focus_event>(x...); });
			ctx->win->set_framebuffer_resize_callback([](auto, auto ... x) { bus->fire<window_framebuffer_resize_event>(x...); });
			ctx->win->set_iconify_callback([](auto, auto ... x) { bus->fire<window_iconify_event>(x...); });
			ctx->win->set_key_callback([](auto, auto ... x) { bus->fire<window_key_event>(x...); });
			ctx->win->set_maximize_callback([](auto, auto ... x) { bus->fire<window_maximize_event>(x...); });
			ctx->win->set_mouse_callback([](auto, auto ... x) { bus->fire<window_mouse_event>(x...); });
			ctx->win->set_position_callback([](auto, auto ... x) { bus->fire<window_position_event>(x...); });
			ctx->win->set_refresh_callback([](auto, auto ... x) { bus->fire<window_refresh_event>(x...); });
			ctx->win->set_resize_callback([](auto, auto ... x) { bus->fire<window_resize_event>(x...); });
			ctx->win->set_scroll_callback([](auto, auto ... x) { bus->fire<window_scroll_event>(x...); });
		}

		// imgui
		ImGui::SetAllocatorFunctions(
			[](size_t s, void * u) { return ((memory_manager *)u)->allocate(s); },
			[](void * p, void * u) { return ((memory_manager *)u)->deallocate(p); },
			ctx->mem);
		m_imgui.reset(ImGui::CreateContext());
		ML_assert(m_imgui);
		m_imgui->IO.LogFilename = nullptr;
		m_imgui->IO.IniFilename = nullptr;
		m_imgui->IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		m_imgui->IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		m_imgui->IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		ML_assert(ImGui_Startup(ctx->win, client_prefs["callbacks"]));
		client_prefs["menu_enabled"	].get_to(m_menu_enabled);
		client_prefs["dock_enabled"	].get_to(m_dock_enabled);
		client_prefs["dock_title"	].get_to(m_dock_title);
		client_prefs["dock_border"	].get_to(m_dock_border);
		client_prefs["dock_rounding"].get_to(m_dock_rounding);
		client_prefs["dock_alpha"	].get_to(m_dock_alpha);
		client_prefs["dock_padding"	].get_to(m_dock_padding);
		client_prefs["dock_size"	].get_to(m_dock_size);
		if (client_prefs.contains("gui_style")) {
			if (client_prefs["gui_style"].is_string()) {
				ImGui_LoadStyle(ctx->io->path2(client_prefs["gui_style"]));
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
				auto const path{ ctx->io->path2(e["path"]).string() };
				auto const file{ std::fopen(path.c_str(), "r") };
				PyRun_AnyFileExFlags(file, path.c_str(), true, nullptr);
			}
		}
	}

	void client_runtime::do_shutdown()
	{
		ImGui_Shutdown(get_window(), m_imgui.release());

		ML_assert(Py_FinalizeEx() == EXIT_SUCCESS);
	}

	void client_runtime::do_idle()
	{
		ML_scope(&io = *get_io()) { // timers
			++io.frame_count;
			io.delta_time = io.loop_timer.elapsed();
			io.loop_timer.restart();
			auto const dt{ (float_t)io.delta_time.count() };
			io.fps_accum += dt - io.fps_times[io.fps_index];
			io.fps_times[io.fps_index] = dt;
			io.fps_index = (io.fps_index + 1) % io.fps_times.size();
			io.fps = (0.f < io.fps_accum)
				? 1.f / (io.fps_accum / (float_t)io.fps_times.size())
				: FLT_MAX;
		};

		native_window::poll_events();

		get_bus()->fire<client_idle_event>(this);

		ImGui_DoFrame(get_window(), m_imgui.get(), [&]() noexcept { do_gui(); });

		if (get_window()->has_hints(window_hints_doublebuffer))
		{
			native_window::swap_buffers(get_window()->get_handle());
		}
	}

	void client_runtime::do_gui()
	{
		ML_ImGui_ScopeID(this);

		// CLIENT DOCKSPACE
		if (m_dock_enabled && (m_imgui->IO.ConfigFlags & ImGuiConfigFlags_DockingEnable))
		{
			ImGuiViewport const * vp{ ImGui::GetMainViewport() };
			ImGui::SetNextWindowPos(vp->Pos);
			ImGui::SetNextWindowSize(vp->Size);
			ImGui::SetNextWindowViewport(vp->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, m_dock_rounding);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, m_dock_border);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_dock_padding);
			ImGui::SetNextWindowBgAlpha(m_dock_alpha);
			if (!ImGuiExt::DrawPanel(m_dock_title.c_str(), &m_dock_enabled,
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoBringToFrontOnFocus |
				ImGuiWindowFlags_NoNavFocus |
				ImGuiWindowFlags_NoDocking |
				ImGuiWindowFlags_NoBackground |
				(m_menu_enabled ? ImGuiWindowFlags_MenuBar : 0),
			[&]() noexcept
			{
				ImGui::PopStyleVar(3);
				get_bus()->fire<client_dock_event>(this);
				ImGui::DockSpace(
					ImGui::GetID(m_dock_title.c_str()),
					m_dock_size,
					ImGuiDockNodeFlags_PassthruCentralNode |
					m_dock_flags);
			}))
			{
				ImGui::PopStyleVar(3);
			}
		}

		// CLIENT MENUBAR
		if (m_menu_enabled && ImGui::BeginMainMenuBar())
		{
			get_bus()->fire<client_menu_event>(this);

			ImGui::EndMainMenuBar();
		}

		// CLIENT GUI
		get_bus()->fire<client_imgui_event>(m_imgui.get());
	}

	void client_runtime::on_event(event && value)
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