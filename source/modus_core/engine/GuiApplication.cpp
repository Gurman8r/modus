
#include <modus_core/engine/GuiApplication.hpp>
#include <modus_core/events/InputEvents.hpp>
#include <modus_core/events/WindowEvents.hpp>
#include <modus_core/events/EngineEvents.hpp>
#include <modus_core/events/ImGuiEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	gui_application::gui_application(int32 argc, char * argv[], json const & j, allocator_type alloc)
		: core_application	{ argc, argv, j, alloc }
		, event_listener	{ &m_dispatcher }
		, m_dispatcher		{ alloc }
		, m_window			{ alloc }
		, m_renderer		{ alloc }
		, m_imgui			{}
		, m_dock_builder	{}
		, m_main_menu_bar	{}
		, m_loop			{ alloc }
		, m_scene			{}

		, m_main_timer		{ true }
		, m_loop_timer		{}
		, m_loop_delta		{}
		, m_loop_index		{}
		, m_fps_value		{}
		, m_fps_accum		{}
		, m_fps_index		{}
		, m_fps_times		{ 120, 0.f, alloc }
		, m_input			{}
	{
		ML_verify(begin_singleton<gui_application>(this));

		static ML_block(&) { ML_verify(platform::initialize()); };

		platform::set_error_callback([](int32 code, cstring desc) { debug::failure("{0}: {1}", code, desc); });

		ImGui::SetAllocatorFunctions(
			[](size_t s, auto u) { return ((memory_manager *)u)->allocate(s); },
			[](void * p, auto u) { return ((memory_manager *)u)->deallocate(p); },
			ML_singleton(memory_manager));
		m_imgui.reset(ImGui::CreateContext());
		m_imgui->IO.LogFilename = "";
		m_imgui->IO.IniFilename = "";
		m_imgui->IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		m_imgui->IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		m_imgui->IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		m_loop.set_condition(&native_window::is_open, get_window());
		m_loop.set_enter_callback([&]() { on_enter(); });
		m_loop.set_exit_callback([&]() { on_exit(); });
		m_loop.set_idle_callback([&]() { on_idle(); });

		subscribe
		<
			char_event,
			key_event,
			mouse_button_event,
			mouse_pos_event,
			mouse_wheel_event
		>();
	}

	gui_application::~gui_application() noexcept
	{
		if (m_scene) { m_scene.reset(); }

		_ML ImGui_Shutdown();

		ImGui::DestroyContext(m_imgui.release());

		unsubscribe(); // manual unsubscribe required because we own the bus

		ML_verify(end_singleton<gui_application>(this));

		static ML_defer(&) { platform::finalize(); };
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void gui_application::on_enter()
	{
		// setup window
		ML_verify(has_attr("window"));
		json & j_window{ attr("window") };
		ML_verify(m_window.open(
			j_window.contains("title") ? j_window["title"] : app_name(),
			j_window.contains("video") ? j_window["video"] : video_mode{},
			j_window.contains("context") ? j_window["context"] : context_settings{},
			j_window.contains("hints") ? j_window["hints"] : window_hints_default));
		{
			static event_bus * b{}; b = &m_dispatcher;
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
		ML_verify(j_window.contains("graphics"));
		json & j_graphics{ j_window["graphics"] };
		ML_verify(m_renderer.initialize(j_graphics));

		// setup imgui
		ML_verify(has_attr("imgui"));
		json & j_imgui{ attr("imgui") };
		ML_verify(_ML ImGui_Init(m_window.get_handle(), true));
		if (j_imgui.contains("style")) {
			json & j_style{ j_imgui["style"] };
			if (j_style.contains("path")) {
				ImGui_LoadStyle(path_to(j_style["path"]));
			}
		}

		m_dispatcher.fire<load_event>(this);
	}

	void gui_application::on_exit()
	{
		m_loop_timer.stop();

		m_dispatcher.fire<unload_event>(this);

		m_main_timer.stop();
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
		platform::poll_events();

		// begin step
		m_dispatcher.fire<begin_step_event>(this);

		// imgui
		ImGuiContext * const gui{ get_imgui_context() };
		_ML ImGui_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
		{
			ImGuiExt_ScopeID(this);

			m_dock_builder.SetWindowFlag(ImGuiWindowFlags_MenuBar, m_main_menu_bar.IsOpen);

			m_dock_builder.Draw(gui->Viewports[0], [&](ImGuiExt::Dockspace * d) noexcept
			{
				if (ImGuiID const id{ d->GetID() }; !d->GetNode(id))
				{
					d->RemoveNode(id);

					d->AddNode(id, d->DockNodeFlags);

					m_dispatcher.fire<dock_builder_event>(d);

					d->Finish(id);
				}
			});

			m_main_menu_bar.Draw([&](ImGuiExt::MainMenuBar * m) noexcept
			{
				m_dispatcher.fire<main_menu_bar_event>(m);
			});

			m_dispatcher.fire<imgui_event>(gui);
		}
		ImGui::Render();

		// clear screen
		m_renderer.get_context()->execute([&](gfx::render_context * ctx) noexcept
		{
			ctx->set_viewport({ {}, (vec2)m_window.get_framebuffer_size() });
			ctx->set_clear_color(colors::black);
			ctx->clear(gfx::clear_flags_color);
		});

		// render imgui
		_ML ImGui_RenderDrawData(&m_imgui->Viewports[0]->DrawDataP);

		// imgui platform windows
		if (m_imgui->IO.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			auto const backup{ platform::get_context_current() };
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			platform::make_context_current(backup);
		}

		// swap buffers
		if (m_window.has_hints(window_hints_doublebuffer)) {
			platform::swap_buffers(m_window.get_handle());
		}

		// end step
		m_dispatcher.fire<end_step_event>(this);
		static vec2 last_mouse_pos{};
		m_input.mouse_delta = m_input.mouse_pos - last_mouse_pos;
		last_mouse_pos = m_input.mouse_pos;
		m_input.mouse_wheel = 0.f;
		m_loop_delta = m_loop_timer.elapsed();
	}

	void gui_application::on_event(event const & value)
	{
		switch (value)
		{
		case char_event::ID: {
			auto const & ev{ (char_event const &)value };
			m_input.last_char = ev.value;
		} break;

		case key_event::ID: {
			auto const & ev{ (key_event const &)value };
			m_input.keys[ev.key] = !ev.is_release();
			if (ev.is_release()) { m_input.key_timers[ev.key].stop(); }
			else if (ev.is_press()) { m_input.key_timers[ev.key].restart(); }
		} break;

		case mouse_button_event::ID: {
			auto const & ev{ (mouse_button_event const &)value };
			m_input.mouse[ev.button] = !ev.is_release();
			if (ev.is_release()) { m_input.mouse_timers[ev.button].stop(); }
			else if (ev.is_press()) { m_input.mouse_timers[ev.button].restart(); }
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