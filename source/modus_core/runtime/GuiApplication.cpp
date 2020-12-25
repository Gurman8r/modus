#include <modus_core/runtime/GuiApplication.hpp>
#include <modus_core/events/InputEvents.hpp>
#include <modus_core/events/WindowEvents.hpp>
#include <modus_core/events/RuntimeEvents.hpp>
#include <modus_core/events/ImGuiEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	gui_application::gui_application(int32 argc, char * argv[], json const & argj, allocator_type alloc)
		: core_application	{ argc, argv, argj, alloc }
		, m_loop			{ alloc }
		, m_window			{ alloc }
		, m_render_device	{}
		, m_imgui_context	{}
		, m_dock_builder	{}
		, m_main_menu_bar	{}
		, m_active_scene	{}

		, m_loop_timer		{}
		, m_loop_delta		{}
		, m_loop_index		{}
		, m_fps_value		{}
		, m_fps_accum		{}
		, m_fps_index		{}
		, m_fps_times		{ 120, 0.f, alloc }
		, m_input			{}
	{
		ML_verify(ML_begin_global(gui_application, this));

		subscribe<
			char_event,
			key_event,
			mouse_button_event,
			mouse_pos_event,
			mouse_wheel_event
		>();

		ImGui::SetAllocatorFunctions(
			[](size_t s, auto u) { return ((memory_manager *)u)->allocate(s); },
			[](void * p, auto u) { return ((memory_manager *)u)->deallocate(p); },
			ML_get_global(memory_manager));
		set_imgui_context(ImGui::CreateContext());
		m_imgui_context->IO.LogFilename = m_imgui_context->IO.IniFilename = NULL;
		m_imgui_context->IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		m_imgui_context->IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		m_imgui_context->IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		m_loop.set_condition(&native_window::is_open, get_window());
		m_loop.set_enter_callback([&]() { on_enter(); });
		m_loop.set_exit_callback([&]() { on_exit(); });
		m_loop.set_idle_callback([&]() { on_idle(); });
	}

	gui_application::~gui_application() noexcept
	{
		if (m_active_scene) { m_active_scene.reset(); }

		_ML ImGui_Shutdown();

		ImGui::DestroyContext(m_imgui_context);

		gfx::destroy_device(m_render_device);

		ML_verify(ML_end_global(gui_application, this));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void gui_application::on_enter()
	{
		// setup window
		ML_assert(has_attr("window"));
		json & j_window{ *attr("window") };
		ML_verify(m_window.open
		(
			j_window.contains("title") ? j_window["title"] : app_name(),
			j_window.contains("display") ? j_window["display"] : video_mode{},
			j_window.contains("graphics") ? j_window["graphics"] : context_settings{},
			j_window.contains("hints") ? j_window["hints"] : window_hints_default
		));
		{
			static event_bus * b{}; b = get_bus();
			m_window.set_char_callback([](auto w, auto ... x) { b->fire<char_event>(x...); });
			m_window.set_char_mods_callback([](auto w, auto ... x) { b->fire<char_mods_event>(x...); });
			m_window.set_key_callback([](auto w, auto ... x) { b->fire<key_event>(x...); });
			m_window.set_mouse_button_callback([](auto w, auto ... x) { b->fire<mouse_button_event>(x...); });
			m_window.set_mouse_enter_callback([](auto w, auto ... x) { b->fire<mouse_enter_event>(x...); });
			m_window.set_mouse_pos_callback([](auto w, auto ... x) { b->fire<mouse_pos_event>(x...); });
			m_window.set_scroll_callback([](auto w, auto ... x) { b->fire<mouse_wheel_event>(x...); });
			
			m_window.set_close_callback([](auto w, auto ... x) { b->fire<window_close_event>(x...); });
			m_window.set_content_scale_callback([](auto w, auto ... x) { b->fire<window_content_scale_event>(x...); });
			m_window.set_drop_callback([](auto w, auto ... x) { b->fire<window_drop_event>(x...); });
			m_window.set_focus_callback([](auto w, auto ... x) { b->fire<window_focus_event>(x...); });
			m_window.set_framebuffer_resize_callback([](auto w, auto ... x) { b->fire<window_framebuffer_resize_event>(x...); });
			m_window.set_iconify_callback([](auto w, auto ... x) { b->fire<window_iconify_event>(x...); });
			m_window.set_maximize_callback([](auto w, auto ... x) { b->fire<window_maximize_event>(x...); });
			m_window.set_position_callback([](auto w, auto ... x) { b->fire<window_position_event>(x...); });
			m_window.set_refresh_callback([](auto w, auto ... x) { b->fire<window_refresh_event>(x...); });
			m_window.set_resize_callback([](auto w, auto ... x) { b->fire<window_resize_event>(x...); });
		}

		// setup graphics
		context_settings cs{};
		if (auto a{ attr("window", "graphics") }) { a->get_to(cs); }
		set_render_device(gfx::render_device::create({ cs.api }));
		set_render_context(get_render_device()->new_context(cs));
		get_render_context()->execute([](gfx::render_context * ctx) noexcept
		{
			ctx->set_alpha_state({});
			ctx->set_blend_state({});
			ctx->set_cull_state({});
			ctx->set_depth_state({});
			ctx->set_stencil_state({});
		});

		// setup imgui
		ML_assert(has_attr("imgui"));
		json & j_imgui{ *attr("imgui") };
		ML_verify(_ML ImGui_Init(m_window.get_handle(), true));
		if (j_imgui.contains("style")) {
			json & j_style{ j_imgui["style"] };
			if (j_style.contains("path")) {
				ImGui_LoadStyle(path_to(j_style["path"]));
			}
		}

		get_bus()->fire<load_event>(this);
	}

	void gui_application::on_exit()
	{
		m_loop_timer.stop();

		get_bus()->fire<unload_event>(this);
	}

	void gui_application::on_idle()
	{
		// update fps
		m_loop_timer.restart();
		float32 const dt{ m_loop_delta.count() };
		m_fps_accum += dt - m_fps_times[m_fps_index];
		m_fps_times[m_fps_index] = dt;
		m_fps_index = (m_fps_index + 1) % m_fps_times.size();
		m_fps_value = (0.f < m_fps_accum) ? (1.f / (m_fps_accum / (float32)m_fps_times.size())) : FLT_MAX;

		// poll events
		native_window::poll_events();

		// begin frame
		get_bus()->fire<begin_frame_event>(this);

		// imgui
		_ML ImGui_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
		{
			ImGuiExt_ScopeID(this);

			m_dock_builder.SetWindowFlag(ImGuiWindowFlags_MenuBar, m_main_menu_bar.IsOpen);

			m_dock_builder.Draw(m_imgui_context->Viewports[0], [&](ImGuiExt::Dockspace * d) noexcept
			{
				if (ImGuiID const id{ d->GetID() }; !d->GetNode(id))
				{
					d->RemoveNode(id);
					d->AddNode(id, d->DockNodeFlags);
					get_bus()->fire<dock_builder_event>(d);
					d->Finish(id);
				}
			});

			m_main_menu_bar.Draw([&](ImGuiExt::MainMenuBar * m) noexcept
			{
				get_bus()->fire<main_menu_bar_event>(m);
			});

			get_bus()->fire<imgui_event>(m_imgui_context);
		}
		ImGui::Render();

		// clear screen
		get_render_context()->execute([&](gfx::render_context * ctx) noexcept
		{
			ctx->set_viewport({ {}, (vec2)m_window.get_framebuffer_size() });
			ctx->set_clear_color(colors::black);
			ctx->clear(gfx::clear_flags_color);
		});

		// render imgui
		_ML ImGui_RenderDrawData(&m_imgui_context->Viewports[0]->DrawDataP);

		// imgui platform windows
		if (m_imgui_context->IO.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			auto const backup{ native_window::get_context_current() };
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			native_window::make_context_current(backup);
		}

		// swap buffers
		if (m_window.has_hints(window_hints_doublebuffer)) {
			native_window::swap_buffers(m_window.get_handle());
		}

		// update inputs
		m_input.mouse_delta = m_input.mouse_pos - m_input.last_mouse_pos;
		m_input.last_mouse_pos = m_input.mouse_pos;
		m_input.mouse_wheel = 0.f;
		for (size_t i = 0; i < mouse_button_MAX; ++i) {
			m_input.mouse_times[i] = (m_input.mouse_down[i] ? (m_input.mouse_times[i] < 0.f ? 0.f : m_input.mouse_times[i] + dt) : -1.f);
		}
		for (size_t i = 0; i < keycode_MAX; ++i) {
			m_input.key_times[i] = (m_input.keys_down[i] ? (m_input.key_times[i] < 0.f ? 0.f : m_input.key_times[i] + dt) : -1.f);
		}

		// end frame
		get_bus()->fire<end_frame_event>(this);
		m_loop_delta = m_loop_timer.elapsed();
	}

	void gui_application::on_event(event const & value)
	{
		core_application::on_event(value);
		switch (value)
		{
		case char_event::ID: {
			auto const & ev{ (char_event const &)value };
			m_input.last_char = ev.value;
		} break;

		case key_event::ID: {
			auto const & ev{ (key_event const &)value };
			m_input.keys_down[ev.key] = !ev.is_release();
		} break;

		case mouse_button_event::ID: {
			auto const & ev{ (mouse_button_event const &)value };
			m_input.mouse_down[ev.button] = !ev.is_release();
		} break;

		case mouse_pos_event::ID: {
			auto const & ev{ (mouse_pos_event const &)value };
			m_input.mouse_pos = { (float32)ev.x, (float32)ev.y };
		} break;

		case mouse_wheel_event::ID: {
			auto const & ev{ (mouse_wheel_event const &)value };
			m_input.mouse_wheel = (float32)ev.y;
		} break;
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