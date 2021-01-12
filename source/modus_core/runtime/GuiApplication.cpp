#include <modus_core/runtime/GuiApplication.hpp>
#include <modus_core/events/InputEvents.hpp>
#include <modus_core/events/WindowEvents.hpp>
#include <modus_core/events/RuntimeEvents.hpp>
#include <modus_core/events/EditorEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	gui_application::gui_application(int32 argc, char * argv[], json const & argj, allocator_type alloc)
		: core_application	{ argc, argv, argj, alloc }
		, m_window			{ alloc }
		, m_render_device	{}
		, m_imgui			{}
		, m_dockspace		{ "##MainDockspace", true, ImGuiDockNodeFlags_AutoHideTabBar }

		, m_loop_timer		{}
		, m_delta_time		{}
		, m_frame_index		{}
		, m_fps				{ 120, alloc }
		, m_input			{}
	{
		ML_ctor_global(gui_application);

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
			ML_memory_manager());
		m_imgui.reset(ImGui::CreateContext());
		m_imgui->IO.LogFilename = NULL;
		m_imgui->IO.IniFilename = NULL;
		m_imgui->IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		m_imgui->IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		m_imgui->IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	}

	gui_application::~gui_application() noexcept
	{
		ML_dtor_global(gui_application);

		_ML ImGui_Shutdown();

		ImGui::DestroyContext(m_imgui.release());

		gfx::destroy_device(m_render_device.release());
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	int32 gui_application::run()
	{
		on_startup();

		while (m_window.is_open())
		{
			m_loop_timer.restart();
			window_api::poll_events();
			on_idle(m_delta_time);

			_ML ImGui_NewFrame();
			ImGui::NewFrame();
			ImGuizmo::BeginFrame();
			on_gui();
			ImGui::Render();

			on_end_frame();
			m_delta_time = m_loop_timer.elapsed();
		}

		on_shutdown();

		return core_application::run();
	}

	void gui_application::exit(int32 exit_code)
	{
		if (m_window.is_open()) { m_window.set_should_close(true); }

		return core_application::exit(exit_code);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void gui_application::on_startup()
	{
		// setup window
		ML_assert(has_attr("window"));
		json & j_window{ get_attr("window") };
		ML_verify(m_window.open
		(
			j_window.contains("title") ? j_window["title"] : get_app_name(),
			j_window.contains("display") ? j_window["display"] : video_mode{},
			j_window.contains("context") ? j_window["context"] : context_settings{},
			j_window.contains("hints") ? j_window["hints"] : window_hints_default
		));

		// install callbacks
		{
			static event_bus * bus{}; bus = get_bus();
			m_window.set_char_callback([](auto w, auto ... x) { bus->broadcast<char_event>(x...); });
			m_window.set_char_mods_callback([](auto w, auto ... x) { bus->broadcast<char_mods_event>(x...); });
			m_window.set_key_callback([](auto w, auto ... x) { bus->broadcast<key_event>(x...); });
			m_window.set_mouse_button_callback([](auto w, auto ... x) { bus->broadcast<mouse_button_event>(x...); });
			m_window.set_mouse_enter_callback([](auto w, auto ... x) { bus->broadcast<mouse_enter_event>(x...); });
			m_window.set_mouse_pos_callback([](auto w, auto ... x) { bus->broadcast<mouse_pos_event>(x...); });
			m_window.set_scroll_callback([](auto w, auto ... x) { bus->broadcast<mouse_wheel_event>(x...); });
			m_window.set_close_callback([](auto w, auto ... x) { bus->broadcast<window_close_event>(x...); });
			m_window.set_content_scale_callback([](auto w, auto ... x) { bus->broadcast<window_content_scale_event>(x...); });
			m_window.set_drop_callback([](auto w, auto ... x) { bus->broadcast<window_drop_event>(x...); });
			m_window.set_focus_callback([](auto w, auto ... x) { bus->broadcast<window_focus_event>(x...); });
			m_window.set_framebuffer_resize_callback([](auto w, auto ... x) { bus->broadcast<window_framebuffer_resize_event>(x...); });
			m_window.set_iconify_callback([](auto w, auto ... x) { bus->broadcast<window_iconify_event>(x...); });
			m_window.set_maximize_callback([](auto w, auto ... x) { bus->broadcast<window_maximize_event>(x...); });
			m_window.set_position_callback([](auto w, auto ... x) { bus->broadcast<window_position_event>(x...); });
			m_window.set_refresh_callback([](auto w, auto ... x) { bus->broadcast<window_refresh_event>(x...); });
			m_window.set_resize_callback([](auto w, auto ... x) { bus->broadcast<window_resize_event>(x...); });
		}

		// setup graphics
		gfx::spec<gfx::render_context> cs{};
		j_window["context"].get_to(cs);
		m_render_device.reset(gfx::render_device::create({ cs.api }));
		m_render_device->set_context(m_render_device->new_context(cs));
		m_render_device->get_context()->execute([](gfx::render_context * ctx) noexcept
		{
			ctx->set_alpha_state({});
			ctx->set_blend_state({});
			ctx->set_cull_state({});
			ctx->set_depth_state({});
			ctx->set_stencil_state({});
		});

		// setup imgui
		json & j_imgui{ get_attr("imgui") };
		ML_verify(_ML ImGui_Init(m_window.get_handle(), true));
		if (j_imgui.contains("style")) {
			json & j_style{ j_imgui["style"] };
			if (j_style.contains("path")) {
				ImGui_LoadStyle(get_path_to(j_style["path"]));
			}
		}

		get_bus()->broadcast<runtime_startup_event>(this);
	}

	void gui_application::on_shutdown()
	{
		// shutdown event
		get_bus()->broadcast<runtime_shutdown_event>(this);
	}

	void gui_application::on_idle(duration dt)
	{
		// fps tracker
		m_fps.update(dt);

		// handle input
		m_input.mouse_delta = m_input.mouse_pos - m_input.last_mouse_pos;
		m_input.last_mouse_pos = m_input.mouse_pos;
		for (size_t i = 0; i < mouse_button_MAX; ++i) {
			m_input.mouse_down_duration[i] = (m_input.mouse_down[i]
				? (m_input.mouse_down_duration[i] < 0.f
					? 0.f
					: m_input.mouse_down_duration[i] + dt)
				: -1.f);
		}
		for (size_t i = 0; i < keycode_MAX; ++i) {
			m_input.keys_down_duration[i] = (m_input.keys_down[i]
				? (m_input.keys_down_duration[i] < 0.f
					? 0.f
					: m_input.keys_down_duration[i] + dt)
				: -1.f);
		}

		get_bus()->broadcast<runtime_idle_event>(this);
	}

	void gui_application::on_gui()
	{
		bool const main_menu_bar{ (bool)ImGui::FindWindowByName("##MainMenuBar") };
		ML_flag_write(m_dockspace.WindowFlags, ImGuiWindowFlags_MenuBar, main_menu_bar);
		m_dockspace.Draw(m_imgui->Viewports[0], [&](ImGuiExt::Dockspace * d) noexcept
		{
			if (d->BeginBuilder()) {
				get_bus()->broadcast<dockspace_builder_event>(d);
				d->Finish();
			}
		});

		get_bus()->broadcast<runtime_gui_event>(this);
	}

	void gui_application::on_end_frame()
	{
		// clear screen
		get_render_context()->execute([&](gfx::render_context * ctx) noexcept
		{
			ctx->set_viewport({ { 0, 0 }, (vec2)m_window.get_framebuffer_size() });
			ctx->set_clear_color(colors::black);
			ctx->clear(gfx::clear_flags_color);
		});

		// render gui
		_ML ImGui_RenderDrawData(&m_imgui->Viewports[0]->DrawDataP);

		// update gui windows
		if (m_imgui->IO.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			window_handle const backup{ window_api::get_active_window() };
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			window_api::set_active_window(backup);
		}

		// swap buffers
		if (m_window.has_hints(window_hints_doublebuffer)) {
			window_api::swap_buffers(m_window.get_handle());
		}

		// reset inputs
		m_input.mouse_wheel = 0.f;

		// end frame event
		get_bus()->broadcast<runtime_end_frame_event>(this);
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
			m_input.keys_down[ev.key] = ev.action != ML_key_release;
			m_input.is_shift = m_input.keys_down[keycode_left_shift] || m_input.keys_down[keycode_right_shift];
			m_input.is_ctrl = m_input.keys_down[keycode_left_ctrl] || m_input.keys_down[keycode_right_ctrl];
			m_input.is_alt = m_input.keys_down[keycode_left_alt] || m_input.keys_down[keycode_right_alt];
			m_input.is_super = m_input.keys_down[keycode_left_super] || m_input.keys_down[keycode_right_super];
		} break;

		case mouse_button_event::ID: {
			auto const & ev{ (mouse_button_event const &)value };
			m_input.mouse_down[ev.button] = ev.action != ML_key_release;
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

	ML_impl_global(gui_application) get_global() { return g_gui_application; }

	ML_impl_global(gui_application) set_global(gui_application * value) { return g_gui_application = value; }
}