#include <modus_core/client/ClientRuntime.hpp>
#include <modus_core/client/ClientEvents.hpp>
#include <modus_core/client/ImGuiExt.hpp>
#include <modus_core/graphics/RenderWindow.hpp>
#include <modus_core/embed/Python.hpp>
#include <modus_core/window/WindowEvents.hpp>

// CLIENT DOCKSPACE
namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	client_dockspace::client_dockspace(client_context * ctx) noexcept
		: client_object	{ ctx }
		, enabled		{}
		, title			{ ctx->mem->get_allocator() }
		, border		{}
		, rounding		{}
		, alpha			{}
		, padding		{}
		, size			{}
		, flags			{ ImGuiDockNodeFlags_AutoHideTabBar }
		, nodes			{ ctx->mem->get_allocator() }
	{
	}

	void client_dockspace::configure(json const & j)
	{
		j["enabled"	].get_to(enabled);
		j["title"	].get_to(title);
		j["border"	].get_to(border);
		j["rounding"].get_to(rounding);
		j["alpha"	].get_to(alpha);
		j["padding"	].get_to(padding);
		j["size"	].get_to(size);
		j["nodes"	].get_to(nodes);
	}

	uint32_t client_dockspace::begin_builder()
	{
		if (uint32_t root{ ImGui::GetID(title.c_str()) }
		; ImGui::DockBuilderGetNode(root)) { return NULL; }
		else
		{
			ImGui::DockBuilderRemoveNode(root);
			ImGui::DockBuilderAddNode(root, flags);
			return root;
		}
	}

	uint32_t client_dockspace::end_builder(uint32_t root)
	{
		if (root) { ImGui::DockBuilderFinish(root); }
		return root;
	}

	uint32_t client_dockspace::dock(cstring name, uint32_t id)
	{
		if (!name || !id) { return NULL; }
		else
		{
			ImGui::DockBuilderDockWindow(name, id);
			return id;
		}
	}

	uint32_t client_dockspace::split(uint32_t i, uint32_t id, int32_t dir, float_t ratio, uint32_t * value)
	{
		return nodes[(size_t)i] = split(id, dir, ratio, value);
	}

	uint32_t client_dockspace::split(uint32_t id, int32_t dir, float_t ratio, uint32_t * value)
	{
		return this->split(id, dir, ratio, nullptr, value);
	}

	uint32_t client_dockspace::split(uint32_t id, int32_t dir, float_t ratio, uint32_t * out, uint32_t * value)
	{
		return ImGui::DockBuilderSplitNode(id, dir, ratio, out, value);
	}

	void client_dockspace::on_event(event && value)
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// CLIENT MENUBAR
namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	client_menubar::client_menubar(client_context * ctx) noexcept
		: client_object	{ ctx }
		, enabled		{}
		, title			{ ctx->mem->get_allocator() }
	{
	}

	void client_menubar::configure(json const & j)
	{
		j["enabled"	].get_to(enabled);
		j["title"	].get_to(title);
	}

	void client_menubar::on_event(event && value)
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// CLIENT RUNTIME
namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	client_runtime::client_runtime(client_context * ctx)
		: client_object	{ ctx }
		, m_running		{}
		, m_imgui		{ nullptr }
		, m_dock		{ ctx->mem->new_object<client_dockspace>(ctx) }
		, m_menu		{ ctx->mem->new_object<client_menubar>(ctx) }
		, m_plugins		{ ctx->mem->new_object<plugin_manager>(ctx) }
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
		// run check
		if (m_running || !get_window()->is_open()) { return EXIT_FAILURE; }
		else { m_running = true; } ML_defer(&) { m_running = false; };
		
		// enter
		get_bus()->fire<client_enter_event>(this);
		if (!get_window()->is_open()) { return EXIT_FAILURE; }

		// idle
		do { do_idle(); }
		while (get_window()->is_open());

		// exit
		get_bus()->fire<client_exit_event>(this);
		return EXIT_SUCCESS;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void client_runtime::do_startup(client_context * ctx)
	{
		// events
		subscribe<window_key_event>();
		subscribe<window_mouse_event>();
		subscribe<window_cursor_pos_event>();

		// python
		PyObject_SetArenaAllocator(([](auto mres) noexcept {
			static PyObjectArenaAllocator temp{
				mres,
				[](auto mres, size_t size) noexcept {
					return ((pmr::memory_resource *)mres)->allocate(size);
				},
				[](auto mres, void * addr, size_t size) noexcept {
					return ((pmr::memory_resource *)mres)->deallocate(addr, size);
				}
			}; return &temp;
		})(ctx->mem->get_resource()));
		Py_SetProgramName(ctx->io->program_name.c_str());
		Py_SetPythonHome(ctx->io->content_path.c_str());
		Py_InitializeEx(1);
		ML_assert(Py_IsInitialized());

		// window
		ML_assert(ctx->win->open(ctx->io->prefs["window"]));
		if (ctx->io->prefs["client"]["callbacks"])
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
		ML_assert(ImGui_Startup(ctx->win, ctx->io->prefs["client"]["callbacks"]));
		m_menu->configure(ctx->io->prefs["client"]["menu"]);
		m_dock->configure(ctx->io->prefs["client"]["dock"]);
		if (ctx->io->prefs["client"].contains("style")) {
			if (ctx->io->prefs["client"]["style"].is_string()) {
				ImGui_LoadStyle(ctx->io->path2(ctx->io->prefs["client"]["style"]));
			}
		}

		// install plugins
		if (ctx->io->prefs["client"].contains("plugins"))
		{
			for (auto const & e : ctx->io->prefs["client"]["plugins"])
			{
				m_plugins->install(e["path"]);
			}
		}

		// execute scripts
		if (ctx->io->prefs["client"].contains("scripts"))
		{
			for (auto const & e : ctx->io->prefs["client"]["scripts"])
			{
				py::eval_file(ctx->io->path2(e["path"]).string());
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
			io.fps_index = (io.fps_index + 1) % ML_arraysize(io.fps_times);
			io.fps = (0.f < io.fps_accum)
				? 1.f / (io.fps_accum / (float_t)ML_arraysize(io.fps_times))
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
		if (m_dock->enabled && (m_imgui->IO.ConfigFlags & ImGuiConfigFlags_DockingEnable))
		{
			ImGuiViewport const * vp{ ImGui::GetMainViewport() };
			ImGui::SetNextWindowPos(vp->Pos);
			ImGui::SetNextWindowSize(vp->Size);
			ImGui::SetNextWindowViewport(vp->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, m_dock->rounding);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, m_dock->border);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_dock->padding);
			ImGui::SetNextWindowBgAlpha(m_dock->alpha);
			if (!ImGuiExt::DrawWindow(m_dock->title.c_str(), &m_dock->enabled,
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoBringToFrontOnFocus |
				ImGuiWindowFlags_NoNavFocus |
				ImGuiWindowFlags_NoDocking |
				ImGuiWindowFlags_NoBackground |
				(m_menu->enabled ? ImGuiWindowFlags_MenuBar : 0),
			[&]() noexcept
			{
				ImGui::PopStyleVar(3);
				get_bus()->fire<client_dock_event>(m_dock.get());
				ImGui::DockSpace(
					ImGui::GetID(m_dock->title.c_str()),
					m_dock->size,
					ImGuiDockNodeFlags_PassthruCentralNode |
					m_dock->flags);
			}))
			{
				ImGui::PopStyleVar(3);
			}
		}

		// CLIENT MENUBAR
		if (m_menu->enabled && ImGui::BeginMainMenuBar())
		{
			get_bus()->fire<client_menu_event>(m_menu.get());

			ImGui::EndMainMenuBar();
		}

		// CLIENT GUI
		get_bus()->fire<client_gui_event>(m_imgui.get());
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