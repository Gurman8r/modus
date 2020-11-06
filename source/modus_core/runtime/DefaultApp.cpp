#include <modus_core/runtime/DefaultApp.hpp>
#include <modus_core/runtime/RuntimeEvents.hpp>
#include <modus_core/embed/Python.hpp>
#include <modus_core/imgui/ImGuiEvents.hpp>
#include <modus_core/window/WindowEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	default_app::default_app(runtime_context * const ctx) noexcept
		: application	{ ctx }
		, m_imgui		{}
		, m_dockspace	{ "##MainDockspace" }
	{
		// events
		subscribe<window_key_event>();
		subscribe<window_mouse_event>();
		subscribe<window_cursor_pos_event>();

		// loopsys
		set_loop_condition(&render_window::is_open, ctx->window);
		set_enter_callback(&default_app::on_enter, this, ctx);
		set_exit_callback(&default_app::on_exit, this, ctx);
		set_idle_callback(&default_app::on_idle, this, ctx);
	}

	default_app::~default_app() noexcept {}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void default_app::on_enter(runtime_context * const ctx)
	{
		// preferences
		ML_assert(ctx->io->prefs.contains("runtime"));
		auto & runtime_prefs{ ctx->io->prefs["runtime"] };
		bool const install_callbacks{ runtime_prefs["callbacks"] };

		// PYTHON
		ML_assert(!Py_IsInitialized());
		PyObject_SetArenaAllocator(([&temp = PyObjectArenaAllocator{}](auto mres) noexcept {
			temp.ctx = mres;
			temp.alloc = [](auto mres, size_t s) {
				return ((pmr::memory_resource *)mres)->allocate(s);
			};
			temp.free = [](auto mres, void * p, size_t s) {
				return ((pmr::memory_resource *)mres)->deallocate(p, s);
			};
			return &temp;
		})(ctx->memory->get_resource()));
		Py_SetProgramName(ctx->io->program_name.c_str());
		Py_SetPythonHome(ctx->io->content_path.c_str());
		Py_InitializeEx(1);
		ML_assert(Py_IsInitialized());

		// WINDOW
		ML_assert(ctx->io->prefs.contains("window"));
		auto & window_prefs{ ctx->io->prefs["window"] };
		ML_assert(ctx->window->open(
			window_prefs["title"],
			window_prefs["video"],
			window_prefs["context"],
			window_prefs["hints"]));
		if (install_callbacks)
		{
			static struct ML_NODISCARD {
				runtime_context * context;
				ML_NODISCARD auto operator->() const noexcept
				{
					return ML_check(context)->bus;
				}
			} helper; helper.context = ctx;

			ctx->window->set_char_callback([](auto w, auto ... x) { helper->fire<window_char_event>(x...); });
			ctx->window->set_char_mods_callback([](auto w, auto ... x) { helper->fire<window_char_mods_event>(x...); });
			ctx->window->set_close_callback([](auto w, auto ... x) { helper->fire<window_close_event>(x...); });
			ctx->window->set_cursor_enter_callback([](auto w, auto ... x) { helper->fire<window_cursor_enter_event>(x...); });
			ctx->window->set_cursor_pos_callback([](auto w, auto ... x) { helper->fire<window_cursor_pos_event>(x...); });
			ctx->window->set_content_scale_callback([](auto w, auto ... x) { helper->fire<window_content_scale_event>(x...); });
			ctx->window->set_drop_callback([](auto w, auto ... x) { helper->fire<window_drop_event>(x...); });
			ctx->window->set_error_callback([](auto ... x) { helper->fire<window_error_event>(x...); });
			ctx->window->set_focus_callback([](auto w, auto ... x) { helper->fire<window_focus_event>(x...); });
			ctx->window->set_framebuffer_resize_callback([](auto w, auto ... x) { helper->fire<window_framebuffer_resize_event>(x...); });
			ctx->window->set_iconify_callback([](auto w, auto ... x) { helper->fire<window_iconify_event>(x...); });
			ctx->window->set_key_callback([](auto w, auto ... x) { helper->fire<window_key_event>(x...); });
			ctx->window->set_maximize_callback([](auto w, auto ... x) { helper->fire<window_maximize_event>(x...); });
			ctx->window->set_mouse_callback([](auto w, auto ... x) { helper->fire<window_mouse_event>(x...); });
			ctx->window->set_position_callback([](auto w, auto ... x) { helper->fire<window_position_event>(x...); });
			ctx->window->set_refresh_callback([](auto w, auto ... x) { helper->fire<window_refresh_event>(x...); });
			ctx->window->set_resize_callback([](auto w, auto ... x) { helper->fire<window_resize_event>(x...); });
			ctx->window->set_scroll_callback([](auto w, auto ... x) { helper->fire<window_scroll_event>(x...); });
		}

		// IMGUI
		ImGui::SetAllocatorFunctions(
			[](size_t s, void * u) { return ((memory_manager *)u)->allocate(s); },
			[](void * p, void * u) { return ((memory_manager *)u)->deallocate(p); },
			ctx->memory);
		m_imgui.reset(ML_check(ImGui::CreateContext()));
		m_imgui->IO.LogFilename = "";
		m_imgui->IO.IniFilename = "";
		m_imgui->IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		m_imgui->IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		m_imgui->IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		ML_assert(ImGui_Startup(ctx->window, install_callbacks));
		if (runtime_prefs.contains("dockspace")) {
			auto & dock_prefs{ runtime_prefs["dockspace"] };
			dock_prefs["alpha"].get_to(m_dockspace.Alpha);
			dock_prefs["border"].get_to(m_dockspace.Border);
			dock_prefs["padding"].get_to(m_dockspace.Padding);
			dock_prefs["rounding"].get_to(m_dockspace.Rounding);
			dock_prefs["size"].get_to(m_dockspace.Size);
		}
		if (runtime_prefs.contains("guistyle")) {
			auto & style_prefs{ runtime_prefs["guistyle"] };
			if (style_prefs.is_string())
			{
				ImGui_LoadStyle(ctx->io->path2(style_prefs));
			}
		}

		// PLUGINS
		if (runtime_prefs.contains("plugins")) {
			auto & plugin_prefs{ runtime_prefs["plugins"] };
			for (auto const & e : plugin_prefs)
			{
				get_plugins().install(e["path"]);
			}
		}

		// SCRIPTS
		if (runtime_prefs.contains("scripts")) {
			auto & script_prefs{ runtime_prefs["scripts"] };
			for (auto const & e : script_prefs)
			{
				auto const path{ ctx->io->path2(e["path"]).string() };
				PyRun_AnyFileExFlags(std::fopen(path.c_str(), "r"), path.c_str(), true, nullptr);
			}
		}

		// enter event
		ctx->bus->fire<app_enter_event>(this);
	}

	void default_app::on_exit(runtime_context * const ctx)
	{
		// exit event
		ctx->bus->fire<app_exit_event>(this);

		// uninstall plugins
		get_plugins().uninstall_all();

		// imgui
		ImGui_Shutdown(ctx->window, m_imgui.release());

		// python
		ML_assert(Py_FinalizeEx() == EXIT_SUCCESS);
	}

	void default_app::on_idle(runtime_context * const ctx)
	{
		// benchmarks
		auto ML_anon{ idle_benchmarks(ctx->io) };

		// poll events
		ctx->window->poll_events();

		// idle event
		ctx->bus->fire<app_idle_event>(this);

		// imgui
		ImGui_DoFrame(ctx->window, m_imgui.get(), [&]() noexcept
		{
			ML_ImGui_ScopeID(this);

			ML_flag_write(
				m_dockspace.WinFlags,
				ImGuiWindowFlags_MenuBar,
				ImGui::FindWindowByName("##MainMenuBar"));
			m_dockspace(m_imgui->Viewports[0], [&]() noexcept
			{
				// imgui dockspace event
				ctx->bus->fire<imgui_dockspace_event>(&m_dockspace);
			});

			// imgui render event
			ctx->bus->fire<imgui_render_event>(m_imgui.get());
		});

		// swap buffers
		ctx->window->swap_buffers();
	}

	void default_app::on_event(event && value)
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