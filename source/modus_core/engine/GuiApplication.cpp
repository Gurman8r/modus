#include <modus_core/engine/GuiApplication.hpp>
#include <modus_core/events/InputEvents.hpp>
#include <modus_core/events/WindowEvents.hpp>
#include <modus_core/events/EngineEvents.hpp>
#include <modus_core/events/ImGuiEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void on_gui_app_enter(gui_application * app);
	void on_gui_app_exit(gui_application * app);
	void on_gui_app_idle(duration const & dt, gui_application * app);

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	gui_application::gui_application(int32 argc, char * argv[], json const & attributes, allocator_type alloc)
		: core_application	{ argc, argv, attributes, alloc }
		, event_listener	{ &m_dispatcher }
		, m_dispatcher		{ alloc }
		, m_window			{ alloc }
		, m_loop			{ alloc }
		, m_render_device	{}
		, m_imgui_context	{}
		, m_dockspace		{}
		, m_menubar			{}
		, m_fps_tracker		{}
	{
		ML_verify(begin_singleton<gui_application>(this));

		static ML_block(&)
		{
			ML_verify(window_context::initialize());

			window_context::set_error_callback([](int32 code, cstring desc)
			{
				debug::failure("{0}: {1}", code, desc);
			});
		};

		ImGui::SetAllocatorFunctions(
			[](size_t s, auto u) { return ((memory_manager *)u)->allocate(s); },
			[](void * p, auto u) { return ((memory_manager *)u)->deallocate(p); },
			get_global<memory_manager>());
		set_imgui_context(ImGui::CreateContext());

		m_loop.set_condition(&native_window::is_open, &m_window);
		m_loop.set_enter_callback(&on_gui_app_enter, this);
		m_loop.set_exit_callback(&on_gui_app_exit, this);
		m_loop.set_idle_callback(&on_gui_app_idle, this);
	}

	gui_application::~gui_application() noexcept
	{
		ML_verify(end_singleton<gui_application>(this));

		_ML ImGui_Shutdown();

		ImGui::DestroyContext(m_imgui_context.release());

		gfx::destroy_device(m_render_device.release());

		unsubscribe(); // manual unsubscribe required because we own the bus

		static ML_defer(&) { window_context::finalize(); };
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void gui_application::on_event(event const & value)
	{
		switch (value)
		{
		case char_event::ID: {
			auto && ev{ (char_event &&)value };
		} break;

		case key_event::ID: {
			auto && ev{ (key_event &&)value };
		} break;

		case mouse_button_event::ID: {
			auto && ev{ (mouse_button_event &&)value };
		} break;

		case mouse_pos_event::ID: {
			auto && ev{ (mouse_pos_event &&)value };
		} break;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void on_gui_app_enter(gui_application * app)
	{
		ML_verify(app);
		auto const bus{ app->get_bus() };
		auto const win{ app->get_window() };
		auto const dev{ app->get_render_device() };
		auto const gui{ app->get_imgui_context() };
		auto const menu{ app->get_menubar() };
		auto const dock{ app->get_dockspace() };

		// window settings
		ML_verify(app->has_attr("window"));
		json & j_window{ app->attr("window") };
		if (!j_window.contains("title")) { j_window["title"] = app->app_name(); }

		// open window
		auto const ws{ (window_settings)j_window };
		ML_verify(win->open(ws.title, ws.video, ws.context, ws.hints));

		// window callbacks
		if (j_window.contains("callbacks") && j_window["callbacks"].get<bool>())
		{
			static event_bus * b{}; b = bus;
			win->set_char_callback([](auto w, auto ... x) { b->fire<char_event>(x...); });
			win->set_char_mods_callback([](auto w, auto ... x) { b->fire<char_mods_event>(x...); });
			win->set_close_callback([](auto w, auto ... x) { b->fire<window_close_event>(x...); });
			win->set_content_scale_callback([](auto w, auto ... x) { b->fire<window_content_scale_event>(x...); });
			win->set_drop_callback([](auto w, auto ... x) { b->fire<window_drop_event>(x...); });
			win->set_focus_callback([](auto w, auto ... x) { b->fire<window_focus_event>(x...); });
			win->set_framebuffer_resize_callback([](auto w, auto ... x) { b->fire<window_framebuffer_resize_event>(x...); });
			win->set_iconify_callback([](auto w, auto ... x) { b->fire<window_iconify_event>(x...); });
			win->set_key_callback([](auto w, auto ... x) { b->fire<key_event>(x...); });
			win->set_maximize_callback([](auto w, auto ... x) { b->fire<window_maximize_event>(x...); });
			win->set_mouse_button_callback([](auto w, auto ... x) { b->fire<mouse_button_event>(x...); });
			win->set_mouse_enter_callback([](auto w, auto ... x) { b->fire<mouse_enter_event>(x...); });
			win->set_mouse_pos_callback([](auto w, auto ... x) { b->fire<mouse_pos_event>(x...); });
			win->set_position_callback([](auto w, auto ... x) { b->fire<window_position_event>(x...); });
			win->set_refresh_callback([](auto w, auto ... x) { b->fire<window_refresh_event>(x...); });
			win->set_resize_callback([](auto w, auto ... x) { b->fire<window_resize_event>(x...); });
			win->set_scroll_callback([](auto w, auto ... x) { b->fire<window_scroll_event>(x...); });
		}

		// setup renderer
		app->set_render_device(gfx::make_device(ws.context.api));
		app->set_render_context(app->get_render_device()->new_context
		({
			ws.context.api,
			ws.context.major,
			ws.context.minor,
			ws.context.profile,
			ws.context.depth_bits,
			ws.context.stencil_bits,
			ws.context.multisample,
			ws.context.srgb_capable
		}));
		app->get_render_context()->execute([&](gfx::render_context * ctx)
		{
			ctx->set_alpha_state({});
			ctx->set_blend_state({});
			ctx->set_cull_state({});
			ctx->set_depth_state({});
			ctx->set_stencil_state({});
		});

		// setup imgui
		if (app->has_attr("imgui"))
		{
			json & j_imgui{ app->attr("imgui") };

			// configure context
			gui->IO.LogFilename = "";
			gui->IO.IniFilename = "";
			gui->IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			gui->IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			gui->IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
			
			// initialize
			_ML ImGui_Init(
				win->get_handle(),
				j_imgui.contains("callbacks") && j_imgui["callbacks"].get<bool>());

			// guistyle
			if (j_imgui.contains("guistyle")) {
				json & j_guistyle{ j_imgui["guistyle"] };
				if (j_guistyle.contains("path")) {
					ImGui_LoadStyle(app->path_to(j_guistyle["path"]));
				}
			}

			// menubar
			if (j_imgui.contains("menubar")) {
				json & j_menubar{ j_imgui["menubar"] };
				menu->Configure(j_menubar);
			}

			// dockspace
			if (j_imgui.contains("dockspace")) {
				json & j_dockspace{ j_imgui["dockspace"] };
				dock->Configure(j_dockspace);
			}
		}

		bus->fire<app_enter_event>();
	}

	void on_gui_app_exit(gui_application * app)
	{
		ML_verify(app);
		auto const bus{ app->get_bus() };
		auto const win{ app->get_window() };
		auto const dev{ app->get_render_device() };
		auto const gui{ app->get_imgui_context() };
		auto const menu{ app->get_menubar() };
		auto const dock{ app->get_dockspace() };

		bus->fire<app_exit_event>();
	}

	void on_gui_app_idle(duration const & dt, gui_application * app)
	{
		ML_verify(app);
		auto const bus{ app->get_bus() };
		auto const win{ app->get_window() };
		auto const dev{ app->get_render_device() };
		auto const gui{ app->get_imgui_context() };
		auto const menu{ app->get_menubar() };
		auto const dock{ app->get_dockspace() };
		
		// update fps
		(*app->get_fps())(dt);

		// poll events
		window_context::poll_events();

		// idle
		bus->fire<app_idle_event>(dt);

		// imgui
		_ML ImGui_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
		ImGui::PushID(app);
		{
			bus->fire<imgui_begin_event>(gui);

			dock->SetWindowFlag(ImGuiWindowFlags_MenuBar, menu->IsOpen);

			dock->Draw(gui->Viewports[0], [&](auto) noexcept
			{
				bus->fire<dock_builder_event>(dock);
			});

			menu->Draw([&](auto) noexcept
			{
				bus->fire<main_menu_bar_event>(menu);
			});

			bus->fire<imgui_render_event>(gui);

			bus->fire<imgui_end_event>(gui);
		}
		ImGui::PopID();
		ImGui::Render();

		app->get_render_context()->execute([&](gfx::render_context * ctx) noexcept
		{
			ctx->set_viewport(win->get_framebuffer_size());
			ctx->set_clear_color(colors::black);
			ctx->clear(gfx::clear_flags_color);
		});

		_ML ImGui_RenderDrawData(&gui->Viewports[0]->DrawDataP);

		if (gui->IO.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			auto const backup{ window_context::get_active_window() };
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			window_context::set_active_window(backup);
		}

		// swap buffers
		if (win->has_hints(window_hints_doublebuffer)) {
			window_context::swap_buffers(win->get_handle());
		}
		
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// global gui_application
namespace ml::globals
{
	static gui_application * g_gui_application{};

	ML_impl_global(gui_application) get() noexcept
	{
		return g_gui_application;
	}

	ML_impl_global(gui_application) set(gui_application * value) noexcept
	{
		return g_gui_application = value;
	}
}