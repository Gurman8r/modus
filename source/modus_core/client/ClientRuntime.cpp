#include <modus_core/client/ClientRuntime.hpp>
#include <modus_core/client/ClientEvents.hpp>
#include <modus_core/graphics/RenderWindow.hpp>
#include <modus_core/embed/Python.hpp>
#include <modus_core/imgui/ImGuiRuntime.hpp>
#include <modus_core/window/WindowEvents.hpp>

// MENUBAR
namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	client_menubar::client_menubar(client_context * ctx) noexcept
		: client_object{ ctx }
	{
	}

	void client_menubar::configure(json const & j)
	{
		j["enabled"].get_to(enabled);
	}

	void client_menubar::on_event(event && value)
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// DOCKSPACE
namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	client_dockspace::client_dockspace(client_context * ctx) noexcept
		: client_object{ ctx }
		, nodes{ ctx->mem->get_allocator() }
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

// RUNTIME
namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	client_runtime::client_runtime(client_context * ctx)
		: client_object	{ ctx }
		, m_running		{}
		, m_menu		{ ctx->mem->new_object<client_menubar>(ctx) }
		, m_dock		{ ctx->mem->new_object<client_dockspace>(ctx) }
		, m_plugins		{ ctx->mem->new_object<plugin_manager>(ctx) }
	{
		// events
		subscribe<window_key_event>();
		subscribe<window_mouse_event>();
		subscribe<window_cursor_pos_event>();

		// io
		auto & io{ *get_io() };
		auto & prefs{ io.prefs };

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
		Py_SetProgramName(io.program_name.c_str());
		Py_SetPythonHome(io.content_path.c_str());
		Py_InitializeEx(1);
		ML_assert(Py_IsInitialized());

		// window
		if (auto const win{ get_window() }
		; prefs.contains("window") && win->open(prefs["window"]))
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

			// imgui
			get_gui()->configure(prefs["imgui"]);
			m_menu->configure(prefs["client"]["menubar"]);
			m_dock->configure(prefs["client"]["dockspace"]);
		}
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
		
		get_bus()->fire<client_init_event>(this);
		ML_defer(&) { get_bus()->fire<client_exit_event>(this); };
		do
		{
			auto & io{ *get_io() };

			begin_frame(io); ML_defer(&) { end_frame(io); };

			get_bus()->fire<client_idle_event>(this);

			get_gui()->do_frame([&]() noexcept
			{
				// client dockspace
				if (m_dock->enabled && (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable))
				{
					ImGuiViewport const * v{ ImGui::GetMainViewport() };
					ImGui::SetNextWindowPos(v->Pos);
					ImGui::SetNextWindowSize(v->Size);
					ImGui::SetNextWindowViewport(v->ID);
					ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, m_dock->rounding);
					ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, m_dock->border);
					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_dock->padding);
					ImGui::SetNextWindowBgAlpha(m_dock->alpha);
					if (ImGui::Begin(m_dock->title.c_str(), &m_dock->enabled,
						ImGuiWindowFlags_NoTitleBar |
						ImGuiWindowFlags_NoCollapse |
						ImGuiWindowFlags_NoResize |
						ImGuiWindowFlags_NoMove |
						ImGuiWindowFlags_NoBringToFrontOnFocus |
						ImGuiWindowFlags_NoNavFocus |
						ImGuiWindowFlags_NoDocking |
						ImGuiWindowFlags_NoBackground |
						(m_menu->enabled ? ImGuiWindowFlags_MenuBar : 0)
					))
					{
						ImGui::PopStyleVar(3);
						if (m_dock->nodes.empty()) { // fire docking event if nodes are empty
							get_bus()->fire<client_dockspace_event>(m_dock.get());
						}
						ImGui::DockSpace(
							ImGui::GetID(m_dock->title.c_str()),
							m_dock->size,
							ImGuiDockNodeFlags_PassthruCentralNode |
							m_dock->flags);
						ImGui::End();
					}
				}

				// client menubar
				if (m_menu->enabled && ImGui::BeginMainMenuBar())
				{
					get_bus()->fire<client_menubar_event>(m_menu.get());

					ImGui::EndMainMenuBar();
				}

				// client gui
				get_bus()->fire<client_gui_event>(this);
			});
		}
		while (get_window()->is_open());
		return EXIT_SUCCESS;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void client_runtime::begin_frame(client_io & io) noexcept
	{
		io.loop_timer.restart();
		
		auto const dt{ (float_t)io.delta_time.count() };
		
		io.fps_accum += dt - io.fps_times[io.fps_index];
		
		io.fps_times[io.fps_index] = dt;
		
		io.fps_index = (io.fps_index + 1) % io.fps_times.size();
		
		io.fps = (0.f < io.fps_accum) ? 1.f / (io.fps_accum / (float_t)io.fps_times.size()) : FLT_MAX;

		native_window::poll_events();
	}

	void client_runtime::end_frame(client_io & io) noexcept
	{
		++io.frame_count;
		
		io.delta_time = io.loop_timer.elapsed();

		if (get_window()->has_hints(window_hints_doublebuffer))
		{
			native_window::swap_buffers(get_window()->get_handle());
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void client_runtime::on_event(event && value)
	{
		switch (value)
		{
		// keyboard
		case window_key_event::ID: {
			auto && ev{ (window_key_event &&)value };
			get_io()->keyboard[ev.key] = ev.action;
		} break;

		// mouse
		case window_mouse_event::ID: {
			auto && ev{ (window_mouse_event &&)value };
			get_io()->mouse[ev.button] = ev.action;
		} break;

		// cursor pos
		case window_cursor_pos_event::ID: {
			auto && ev{ (window_cursor_pos_event &&)value };
			get_io()->cursor = { ev.x, ev.y };
		} break;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}