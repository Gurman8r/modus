#include <modus_core/engine/MainWindow.hpp>
#include <modus_core/window/WindowEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	main_window::main_window(event_bus * bus, allocator_type alloc) noexcept
		: core_object	{ bus }
		, render_window	{ alloc }
		, m_imgui		{}
		, m_menubar		{ "##MainMenuBar" }
		, m_dockspace	{ "##MainDockspace" }
	{
		ImGui::SetAllocatorFunctions(
			[](size_t s, void * u) { return ((memory_manager *)u)->allocate(s); },
			[](void * p, void * u) { return ((memory_manager *)u)->deallocate(p); },
			get_global<memory_manager>());
		
		m_imgui.reset(ML_check(ImGui::CreateContext()));
		m_imgui->IO.LogFilename = "";
		m_imgui->IO.IniFilename = "";
		m_imgui->IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		m_imgui->IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		m_imgui->IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	}

	main_window::main_window(
		event_bus *					bus,
		ds::string			const & title,
		video_mode			const & vm,
		context_settings	const & cs,
		window_hints_				hints,
		void *						userptr,
		allocator_type				alloc
	) noexcept : main_window{ bus, alloc }
	{
		ML_assert(this->open(title, vm, cs, hints, userptr));
	}

	main_window::~main_window() noexcept
	{
		ImGui::DestroyContext(m_imgui.release());
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool main_window::open(
		ds::string			const & title,
		video_mode			const & vm,
		context_settings	const & cs,
		window_hints_				hints,
		void *						userptr
	)
	{
		// check open
		if (is_open())
		{
			return debug::error("main_window is already open");
		}

		// open base
		if (!render_window::open(title, vm, cs, hints, userptr))
		{
			return debug::error("failed opening main_window");
		}

		// install callbacks
		if (static event_bus * helper; ML_check(helper = get_bus()))
		{
			set_char_callback([](auto w, auto ... x) { helper->fire<window_char_event>(x...); });
			set_char_mods_callback([](auto w, auto ... x) { helper->fire<window_char_mods_event>(x...); });
			set_close_callback([](auto w, auto ... x) { helper->fire<window_close_event>(x...); });
			set_cursor_enter_callback([](auto w, auto ... x) { helper->fire<window_cursor_enter_event>(x...); });
			set_cursor_pos_callback([](auto w, auto ... x) { helper->fire<window_cursor_pos_event>(x...); });
			set_content_scale_callback([](auto w, auto ... x) { helper->fire<window_content_scale_event>(x...); });
			set_drop_callback([](auto w, auto ... x) { helper->fire<window_drop_event>(x...); });
			set_focus_callback([](auto w, auto ... x) { helper->fire<window_focus_event>(x...); });
			set_framebuffer_resize_callback([](auto w, auto ... x) { helper->fire<window_framebuffer_resize_event>(x...); });
			set_iconify_callback([](auto w, auto ... x) { helper->fire<window_iconify_event>(x...); });
			set_key_callback([](auto w, auto ... x) { helper->fire<window_key_event>(x...); });
			set_maximize_callback([](auto w, auto ... x) { helper->fire<window_maximize_event>(x...); });
			set_mouse_callback([](auto w, auto ... x) { helper->fire<window_mouse_event>(x...); });
			set_position_callback([](auto w, auto ... x) { helper->fire<window_position_event>(x...); });
			set_refresh_callback([](auto w, auto ... x) { helper->fire<window_refresh_event>(x...); });
			set_resize_callback([](auto w, auto ... x) { helper->fire<window_resize_event>(x...); });
			set_scroll_callback([](auto w, auto ... x) { helper->fire<window_scroll_event>(x...); });
		}

		// imgui
		if (!initialize_imgui())
		{
			return debug::error("failed starting imgui");
		}

		return true;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool main_window::initialize_imgui(bool install_callbacks)
	{
		return _ML ImGui_Init(get_handle(), install_callbacks);
	}

	void main_window::finalize_imgui()
	{
		_ML ImGui_Shutdown();
	}

	bool main_window::load_imgui_style(fs::path const & path)
	{
		return _ML ImGui_LoadStyle(path, get_imgui()->Style);
	}

	void main_window::begin_imgui_frame()
	{
		get_window_context()->poll_events();

		_ML ImGui_NewFrame();

		ImGui::NewFrame();

		ImGui::PushID(this);
	}

	void main_window::end_imgui_frame()
	{
		ImGui::PopID();

		ImGui::Render();

		get_render_context()->execute(
			gfx::command::set_viewport(get_framebuffer_size()),
			gfx::command::set_clear_color(colors::black),
			gfx::command::clear(gfx::clear_color));

		_ML ImGui_RenderDrawData(&get_imgui()->Viewports[0]->DrawDataP);

		if (get_imgui()->IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			auto const backup_context{ get_window_context()->get_context_current() };
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			get_window_context()->make_context_current(backup_context);
		}

		if (has_hints(window_hints_doublebuffer))
		{
			get_window_context()->swap_buffers(get_handle());
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}