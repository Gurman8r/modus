#include <modus_core/engine/MainWindow.hpp>
#include <modus_core/graphics/Renderer2D.hpp>
#include <modus_core/window/WindowEvents.hpp>
#include <modus_core/imgui/ImGuiEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	main_window::main_window(allocator_type alloc) noexcept
		: render_window	{ alloc }
		, m_imgui		{}
		, m_dockspace	{}
		, m_menubar		{}
	{
		ImGui::SetAllocatorFunctions(
			[](size_t s, auto u) { return ((memory_manager *)u)->allocate(s); },
			[](void * p, auto u) { return ((memory_manager *)u)->deallocate(p); },
			get_global<memory_manager>());
		
		m_imgui.reset(ML_check(ImGui::CreateContext()));
		m_imgui->IO.LogFilename = "";
		m_imgui->IO.IniFilename = "";
		m_imgui->IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		m_imgui->IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		m_imgui->IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	}

	main_window::~main_window() noexcept
	{
		finalize_imgui();

		ImGui::DestroyContext(m_imgui.release());

		renderer2d::finalize();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool main_window::open(
		ds::string			const & title,
		video_mode			const & vm,
		context_settings	const & cs,
		window_hints_				hints
	)
	{
		// open base
		if (!render_window::open(title, vm, cs, hints)) {
			return debug::failure("failed opening render_window");
		}

		// renderer
		if (!renderer2d::initialize()) {
			return debug::failure();
		}

		return true;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void main_window::install_callbacks(event_bus * bus)
	{
		static event_bus * b{};
		if (!(b = bus)) { return; }
		set_char_callback([](auto w, auto ... x) { b->fire<window_char_event>(x...); });
		set_char_mods_callback([](auto w, auto ... x) { b->fire<window_char_mods_event>(x...); });
		set_close_callback([](auto w, auto ... x) { b->fire<window_close_event>(x...); });
		set_cursor_enter_callback([](auto w, auto ... x) { b->fire<window_cursor_enter_event>(x...); });
		set_cursor_pos_callback([](auto w, auto ... x) { b->fire<window_cursor_pos_event>(x...); });
		set_content_scale_callback([](auto w, auto ... x) { b->fire<window_content_scale_event>(x...); });
		set_drop_callback([](auto w, auto ... x) { b->fire<window_drop_event>(x...); });
		set_focus_callback([](auto w, auto ... x) { b->fire<window_focus_event>(x...); });
		set_framebuffer_resize_callback([](auto w, auto ... x) { b->fire<window_framebuffer_resize_event>(x...); });
		set_iconify_callback([](auto w, auto ... x) { b->fire<window_iconify_event>(x...); });
		set_key_callback([](auto w, auto ... x) { b->fire<window_key_event>(x...); });
		set_maximize_callback([](auto w, auto ... x) { b->fire<window_maximize_event>(x...); });
		set_mouse_callback([](auto w, auto ... x) { b->fire<window_mouse_event>(x...); });
		set_position_callback([](auto w, auto ... x) { b->fire<window_position_event>(x...); });
		set_refresh_callback([](auto w, auto ... x) { b->fire<window_refresh_event>(x...); });
		set_resize_callback([](auto w, auto ... x) { b->fire<window_resize_event>(x...); });
		set_scroll_callback([](auto w, auto ... x) { b->fire<window_scroll_event>(x...); });
	}

	bool main_window::initialize_imgui(bool callbacks)
	{
		return _ML ImGui_Init(get_handle(), callbacks);
	}

	void main_window::finalize_imgui()
	{
		_ML ImGui_Shutdown();
	}

	void main_window::begin_frame()
	{
		window_context::poll_events();

		_ML ImGui_NewFrame();

		ImGui::NewFrame();

		ImGui::PushID(this);
	}

	void main_window::end_frame()
	{
		ImGui::PopID();

		ImGui::Render();

		get_render_context()->execute([&](gfx::render_context * ctx) noexcept
		{
			ctx->set_viewport(get_framebuffer_size());
			ctx->set_clear_color(colors::black);
			ctx->clear(gfx::clear_flags_color);
		});

		_ML ImGui_RenderDrawData(&m_imgui->Viewports[0]->DrawDataP);

		if (m_imgui->IO.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			auto const backup{ window_context::get_active_window() };
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			window_context::set_active_window(backup);
		}

		if (has_hints(window_hints_doublebuffer))
		{
			window_context::swap_buffers(get_handle());
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool main_window::load_style(fs::path const & path)
	{
		return _ML ImGui_LoadStyle(path, m_imgui->Style);
	}

	bool main_window::load_style(json const & j)
	{
		return false;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}