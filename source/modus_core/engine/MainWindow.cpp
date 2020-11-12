#include <modus_core/engine/MainWindow.hpp>
#include <modus_core/window/WindowEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	main_window::main_window(event_bus * bus, allocator_type alloc) noexcept
		: core_object	{ bus }
		, render_window	{ alloc }
		, m_imgui		{}
		, m_docker		{ new ImGuiExt::Dockspace{ "##MainDockspace" } }
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
		// check already open
		if (is_open()) {
			return debug::error("main_window is already open");
		}

		// open render_window
		if (!render_window::open(title, vm, cs, hints, userptr)) {
			return debug::error("failed opening main_window");
		}

		// install callbacks
		{
			static struct ML_NODISCARD {
				event_bus * bus;
				ML_NODISCARD auto operator->() const noexcept { return ML_check(bus); }
			} helper; helper.bus = get_bus();

			set_char_callback([](auto w, auto ... x) { helper->fire<window_char_event>(x...); });
			set_char_mods_callback([](auto w, auto ... x) { helper->fire<window_char_mods_event>(x...); });
			set_close_callback([](auto w, auto ... x) { helper->fire<window_close_event>(x...); });
			set_cursor_enter_callback([](auto w, auto ... x) { helper->fire<window_cursor_enter_event>(x...); });
			set_cursor_pos_callback([](auto w, auto ... x) { helper->fire<window_cursor_pos_event>(x...); });
			set_content_scale_callback([](auto w, auto ... x) { helper->fire<window_content_scale_event>(x...); });
			set_drop_callback([](auto w, auto ... x) { helper->fire<window_drop_event>(x...); });
			set_error_callback([](auto ... x) { helper->fire<window_error_event>(x...); });
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
		if (!startup_imgui()) {
			return debug::error("failed starting imgui");
		}

		return true;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void main_window::on_event(event const & value)
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}