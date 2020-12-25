#include "./GLFW.hpp"
#include "./GLFW_Window.hpp"

// GLFW MONITOR
namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	inline monitor make_monitor(GLFWmonitor * m)
	{
		monitor temp{};

		ds::string name{ glfwGetMonitorName(m) };

		int32 width, height;
		glfwGetMonitorPhysicalSize(m, &width, &height);

		void * userptr{ glfwGetMonitorUserPointer(m) };

		return temp;
	}

	monitor const & glfw_monitor::get_primary()
	{
		static monitor temp{ make_monitor(glfwGetPrimaryMonitor()) };
		return temp;
	}

	ds::list<monitor> const & glfw_monitor::get_monitors()
	{
		static ds::list<monitor> temp{};
		if (int32 count; GLFWmonitor ** m{ glfwGetMonitors(&count) })
		{

		}
		return temp;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// GLFW CURSOR
namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	cursor glfw_cursor::create_custom(size_t w, size_t h, byte const * p, int32 x, int32 y)
	{
		GLFWimage temp{ (int32)w, (int32)h, (byte *)p };

		return { (cursor_handle)glfwCreateCursor(&temp, x, y) };
	}

	cursor glfw_cursor::create_standard(cursor_shape_ shape)
	{
		return { (cursor_handle)glfwCreateStandardCursor(std::invoke([shape]() noexcept
		{
			switch (shape)
			{
			default: return 0;
			case cursor_shape_arrow			: return GLFW_ARROW_CURSOR;
			case cursor_shape_ibeam			: return GLFW_IBEAM_CURSOR;
			case cursor_shape_crosshair		: return GLFW_CROSSHAIR_CURSOR;
			case cursor_shape_pointing_hand	: return GLFW_POINTING_HAND_CURSOR;
			case cursor_shape_ew			: return GLFW_RESIZE_EW_CURSOR;
			case cursor_shape_ns			: return GLFW_RESIZE_NS_CURSOR;
			case cursor_shape_nesw			: return GLFW_RESIZE_NESW_CURSOR;
			case cursor_shape_nwse			: return GLFW_RESIZE_NWSE_CURSOR;
			case cursor_shape_resize_all	: return GLFW_RESIZE_ALL_CURSOR;
			case cursor_shape_not_allowed	: return GLFW_NOT_ALLOWED_CURSOR;

			// glfw doesn't have these
			case cursor_shape_hresize		: return GLFW_HRESIZE_CURSOR;
			case cursor_shape_vresize		: return GLFW_VRESIZE_CURSOR;
			case cursor_shape_hand			: return GLFW_HAND_CURSOR;
			}
		})) };
	}

	void glfw_cursor::destroy(cursor const & value)
	{
		glfwDestroyCursor((GLFWcursor *)value.ID);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// GLFW CONTEXT
namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool glfw_platform::initialize()
	{
		return glfwInit();
	}

	bool glfw_platform::finalize()
	{
		glfwTerminate();
		return true;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	window_handle glfw_platform::get_context_current()
	{
		return (window_handle)glfwGetCurrentContext();
	}

	void glfw_platform::make_context_current(window_handle value)
	{
		glfwMakeContextCurrent((GLFWwindow *)value);
	}

	void glfw_platform::poll_events()
	{
		glfwPollEvents();
	}

	void glfw_platform::swap_buffers(window_handle value)
	{
		glfwSwapBuffers((GLFWwindow *)value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// GLFW WINDOW
namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
	glfw_window::glfw_window(allocator_type alloc) noexcept
		: m_title	{ alloc }
		, m_window	{}
		, m_monitor	{}
		, m_hints	{}
	{
		static ML_block(&) { ML_verify(glfw_platform::initialize()); };
	}

	glfw_window::~glfw_window()
	{
		glfwDestroyWindow(m_window);

		static ML_defer(&) { ML_verify(glfw_platform::finalize()); };
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool glfw_window::open(
		ds::string			const & title,
		video_mode			const & vm,
		context_settings	const & cs,
		window_hints_				hints
	)
	{
		// check already open
		if (is_open()) { return debug::failure("window is already open"); }

		// title
		if ((m_title = title).empty()) { m_title = "GLFW"; }

		// hints
		m_hints = hints;

		// context hints
		glfwWindowHint(GLFW_CLIENT_API, std::invoke([api = cs.api]() noexcept
		{
			switch (api)
			{
			case context_api_opengl	: return GLFW_OPENGL_API;
			case context_api_vulkan	:
			case context_api_directx:
			case context_api_unknown:
			default					: return GLFW_NO_API;
			}
		}));
		glfwWindowHint(GLFW_OPENGL_PROFILE, std::invoke([profile = cs.profile]() noexcept
		{
			switch (profile)
			{
			case context_profile_core	: return GLFW_OPENGL_CORE_PROFILE;
			case context_profile_compat	: return GLFW_OPENGL_COMPAT_PROFILE;
			case context_profile_debug	: return GLFW_OPENGL_DEBUG_CONTEXT;
			case context_profile_any	:
			default						: return GLFW_OPENGL_ANY_PROFILE;
			}
		}));
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,	cs.major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,	cs.minor);

		// window hints
		glfwWindowHint(GLFW_AUTO_ICONIFY,	hints & window_hints_auto_iconify);
		glfwWindowHint(GLFW_CENTER_CURSOR,	hints & window_hints_center_cursor);
		glfwWindowHint(GLFW_DECORATED,		hints & window_hints_decorated);
		glfwWindowHint(GLFW_DOUBLEBUFFER,	hints & window_hints_doublebuffer);
		glfwWindowHint(GLFW_FLOATING,		hints & window_hints_floating);
		glfwWindowHint(GLFW_FOCUS_ON_SHOW,	hints & window_hints_focus_on_show);
		glfwWindowHint(GLFW_FOCUSED,		hints & window_hints_focused);
		glfwWindowHint(GLFW_MAXIMIZED,		hints & window_hints_maximized);
		glfwWindowHint(GLFW_RESIZABLE,		hints & window_hints_resizable);
		glfwWindowHint(GLFW_VISIBLE,		hints & window_hints_visible);

		// monitor hints
		glfwWindowHint(GLFW_REFRESH_RATE,	vm.refresh_rate);

		// framebuffer hints
		glfwWindowHint(GLFW_RED_BITS,		vm.bits_per_pixel[0]);
		glfwWindowHint(GLFW_GREEN_BITS,		vm.bits_per_pixel[1]);
		glfwWindowHint(GLFW_BLUE_BITS,		vm.bits_per_pixel[2]);
		glfwWindowHint(GLFW_ALPHA_BITS,		vm.bits_per_pixel[3]);
		glfwWindowHint(GLFW_DEPTH_BITS,		cs.depth_bits);
		glfwWindowHint(GLFW_STENCIL_BITS,	cs.stencil_bits);
		glfwWindowHint(GLFW_SRGB_CAPABLE,	cs.srgb_capable);

		// create window
		if (!(m_window = glfwCreateWindow(
			vm.resolution[0],
			vm.resolution[1],
			m_title.c_str(), // title
			nullptr, // monitor
			nullptr // share
		)))
		{
			return debug::failure("failed opening glfw_window");
		}
		
		// info
		return true;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void glfw_window::focus()
	{
		glfwFocusWindow(m_window);
	}

	void glfw_window::hide()
	{
		glfwHideWindow(m_window);
	}

	void glfw_window::iconify()
	{
		glfwIconifyWindow(m_window);
	}

	void glfw_window::maximize()
	{
		glfwMaximizeWindow(m_window);
	}

	void glfw_window::restore()
	{
		glfwRestoreWindow(m_window);
	}

	void glfw_window::request_attention()
	{
		glfwRequestWindowAttention(m_window);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	window_callbacks const & glfw_window::get_callbacks() const
	{
		return m_clbk;
	}

	ds::string glfw_window::get_clipboard() const
	{
		return glfwGetClipboardString(m_window);
	}

	vec2 glfw_window::get_content_scale() const
	{
		vec2 temp{};
		glfwGetWindowContentScale(m_window, &temp[0], &temp[1]);
		return temp;
	}

	vec2i glfw_window::get_framebuffer_size() const
	{
		vec2i temp{};
		glfwGetFramebufferSize(m_window, &temp[0], &temp[1]);
		return temp;
	}

	window_handle glfw_window::get_handle() const
	{
		return (window_handle)m_window;
	}

	window_hints_ glfw_window::get_hints() const
	{
		return m_hints;
	}

	int32 glfw_window::get_input_mode(int32 mode) const
	{
		return glfwGetInputMode(m_window, mode);
	}

	int32 glfw_window::get_key(int32 key) const
	{
		return glfwGetKey(m_window, key);
	}

	int32 glfw_window::get_mouse_button(int32 button) const
	{
		return glfwGetMouseButton(m_window, button);
	}

	vec2 glfw_window::get_mouse_pos() const
	{
		vec2d temp{};
		glfwGetCursorPos(m_window, &temp[0], &temp[1]);
		return (vec2)temp;
	}

	window_handle glfw_window::get_native_handle() const
	{
#ifdef ML_os_windows
		return (window_handle)glfwGetWin32Window(m_window);
#else
		return m_window;
#endif
	}

	float32 glfw_window::get_opacity() const
	{
		return glfwGetWindowOpacity(m_window);
	}

	vec2i glfw_window::get_position() const
	{
		vec2i temp{};
		glfwGetWindowPos(m_window, &temp[0], &temp[1]);
		return temp;
	}

	vec2i glfw_window::get_size() const
	{
		vec2i temp{};
		glfwGetWindowSize((GLFWwindow *)m_window, &temp[0], &temp[1]);
		return temp;
	}

	ds::string const & glfw_window::get_title() const
	{
		return m_title;
	}

	void * glfw_window::get_user_pointer() const
	{
		return glfwGetWindowUserPointer((GLFWwindow *)m_window);
	}

	int_rect glfw_window::get_window_frame_size() const
	{
		int32 l, t, r, b;
		glfwGetWindowFrameSize(m_window, &l, &r, &t, &b);
		return { l, t, r - l, b - t };
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool glfw_window::is_auto_iconify() const
	{
		return m_window && glfwGetWindowAttrib(m_window, GLFW_AUTO_ICONIFY);
	}

	bool glfw_window::is_decorated() const
	{
		return m_window && glfwGetWindowAttrib(m_window, GLFW_DECORATED);
	}

	bool glfw_window::is_floating() const
	{
		return m_window && glfwGetWindowAttrib(m_window, GLFW_FLOATING);
	}

	bool glfw_window::is_focus_on_show() const
	{
		return m_window && glfwGetWindowAttrib(m_window, GLFW_FOCUS_ON_SHOW);
	}

	bool glfw_window::is_focused() const
	{
		return m_window && glfwGetWindowAttrib(m_window, GLFW_FOCUSED);
	}

	bool glfw_window::is_hovered() const
	{
		return m_window && glfwGetWindowAttrib(m_window, GLFW_HOVERED);
	}

	bool glfw_window::is_iconified() const
	{
		return m_window && glfwGetWindowAttrib(m_window, GLFW_ICONIFIED);
	}

	bool glfw_window::is_maximized() const
	{
		return m_window && glfwGetWindowAttrib(m_window, GLFW_MAXIMIZED);
	}

	bool glfw_window::is_open() const
	{
		return m_window && !glfwWindowShouldClose(m_window);
	}

	bool glfw_window::is_resizable() const
	{
		return m_window && glfwGetWindowAttrib(m_window, GLFW_RESIZABLE);
	}

	bool glfw_window::is_transparent() const
	{
		return m_window && glfwGetWindowAttrib(m_window, GLFW_TRANSPARENT_FRAMEBUFFER);
	}

	bool glfw_window::is_visible() const
	{
		return m_window && glfwGetWindowAttrib(m_window, GLFW_VISIBLE);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void glfw_window::set_auto_iconify(bool value)
	{
		glfwSetWindowAttrib(m_window, GLFW_AUTO_ICONIFY, value);
	}

	void glfw_window::set_clipboard(ds::string const & value)
	{
		glfwSetClipboardString(m_window, value.c_str());
	}

	void glfw_window::set_cursor(cursor_handle value)
	{
		glfwSetCursor(m_window, (GLFWcursor *)value);
	}
	
	void glfw_window::set_cursor_mode(int32 value)
	{
		set_input_mode(GLFW_CURSOR, std::invoke([value]() noexcept
		{
			switch (value)
			{
			default						: return 0;
			case cursor_mode_normal		: return GLFW_CURSOR_NORMAL;
			case cursor_mode_hidden		: return GLFW_CURSOR_HIDDEN;
			case cursor_mode_disabled	: return GLFW_CURSOR_DISABLED;
			}
		}));
	}

	void glfw_window::set_mouse_pos(vec2d const & value)
	{
		glfwSetCursorPos(m_window, value[0], value[1]);
	}

	void glfw_window::set_decorated(bool value)
	{
		glfwSetWindowAttrib(m_window, GLFW_DECORATED, value);
	}

	void glfw_window::set_floating(bool value)
	{
		glfwSetWindowAttrib(m_window, GLFW_FLOATING, value);
	}

	void glfw_window::set_focus_on_show(bool value)
	{
		glfwSetWindowAttrib(m_window, GLFW_FOCUS_ON_SHOW, value);
	}

	void glfw_window::set_icons(int32 n, size_t w, size_t h, byte const * p)
	{
		GLFWimage temp{ (int32)w, (int32)h, (byte *)p };

		glfwSetWindowIcon(m_window, n, &temp);
	}

	void glfw_window::set_input_mode(int32 mode, int32 value)
	{
		glfwSetInputMode(m_window, mode, value);
	}

	void glfw_window::set_opacity(float32 value)
	{
		glfwSetWindowOpacity(m_window, value);
	}

	void glfw_window::set_position(vec2i const & value)
	{
		glfwSetWindowPos(m_window, value[0], value[1]);
	}

	void glfw_window::set_monitor(monitor_handle value, int_rect const & bounds)
	{
		// WIP

		m_monitor = (GLFWmonitor *)value;
		if (m_monitor)
		{
			auto const vm{ glfwGetVideoMode(m_monitor) };
			glfwSetWindowMonitor(m_window, m_monitor,
				0,
				0,
				vm->width,
				vm->height,
				vm->refreshRate);
		}
		else
		{
			glfwSetWindowMonitor(m_window, m_monitor,
				bounds.left(),
				bounds.top(),
				bounds.width(),
				bounds.height(),
				GLFW_DONT_CARE);
		}
	}

	void glfw_window::set_resizable(bool value)
	{
		glfwSetWindowAttrib(m_window, GLFW_RESIZABLE, value);
	}

	void glfw_window::set_should_close(bool value)
	{
		glfwSetWindowShouldClose(m_window, value);
	}

	void glfw_window::set_size(vec2i const & value)
	{
		glfwSetWindowSize(m_window, value[0], value[1]);
	}

	void glfw_window::set_title(ds::string const & value)
	{
		glfwSetWindowTitle(m_window, (m_title = value).c_str());
	}

	void glfw_window::set_user_pointer(void * value)
	{
		glfwSetWindowUserPointer((GLFWwindow *)m_window, value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	window_char_callback glfw_window::get_char_callback() const
	{
		return m_clbk.on_char;
	}

	window_char_mods_callback glfw_window::get_char_mods_callback() const
	{
		return m_clbk.on_char_mods;
	}

	window_close_callback glfw_window::get_close_callback() const
	{
		return m_clbk.on_close;
	}

	window_content_scale_callback glfw_window::get_content_scale_callback() const
	{
		return m_clbk.on_content_scale;
	}

	window_drop_callback glfw_window::get_drop_callback() const
	{
		return m_clbk.on_drop;
	}

	window_focus_callback glfw_window::get_focus_callback() const
	{
		return m_clbk.on_focus;
	}

	window_framebuffer_resize_callback glfw_window::get_framebuffer_resize_callback() const
	{
		return m_clbk.on_framebuffer_resize;
	}

	window_iconify_callback glfw_window::get_iconify_callback() const
	{
		return m_clbk.on_iconify;
	}

	window_key_callback glfw_window::get_key_callback() const
	{
		return m_clbk.on_key;
	}

	window_maximize_callback glfw_window::get_maximize_callback() const
	{
		return m_clbk.on_maximize;
	}

	window_mouse_button_callback glfw_window::get_mouse_button_callback() const
	{
		return m_clbk.on_mouse_button;
	}

	window_mouse_enter_callback glfw_window::get_mouse_enter_callback() const
	{
		return m_clbk.on_mouse_enter;
	}

	window_mouse_pos_callback glfw_window::get_mouse_pos_callback() const
	{
		return m_clbk.on_mouse_pos;
	}

	window_position_callback glfw_window::get_position_callback() const
	{
		return m_clbk.on_position;
	}

	window_refresh_callback glfw_window::get_refresh_callback() const
	{
		return m_clbk.on_refresh;
	}

	window_resize_callback glfw_window::get_resize_callback() const
	{
		return m_clbk.on_resize;
	}

	window_scroll_callback glfw_window::get_scroll_callback() const
	{
		return m_clbk.on_mouse_wheel;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	window_char_callback glfw_window::set_char_callback(window_char_callback value)
	{
		return reinterpret_cast<window_char_callback>(
			glfwSetCharCallback(m_window,
				reinterpret_cast<GLFWcharfun>(m_clbk.on_char = value)));
	}

	window_char_mods_callback glfw_window::set_char_mods_callback(window_char_mods_callback value)
	{
		return reinterpret_cast<window_char_mods_callback>(
			glfwSetCharModsCallback(m_window,
				reinterpret_cast<GLFWcharmodsfun>(m_clbk.on_char_mods = value)));
	}

	window_close_callback glfw_window::set_close_callback(window_close_callback value)
	{
		return reinterpret_cast<window_close_callback>(
			glfwSetWindowCloseCallback(m_window,
				reinterpret_cast<GLFWwindowclosefun>(m_clbk.on_close = value)));
	}

	window_content_scale_callback glfw_window::set_content_scale_callback(window_content_scale_callback value)
	{
		return reinterpret_cast<window_content_scale_callback>(
			glfwSetWindowContentScaleCallback(m_window,
				reinterpret_cast<GLFWwindowcontentscalefun>(m_clbk.on_content_scale = value)));
	}

	window_drop_callback glfw_window::set_drop_callback(window_drop_callback value)
	{
		return reinterpret_cast<window_drop_callback>(
			glfwSetDropCallback(m_window,
				reinterpret_cast<GLFWdropfun>(m_clbk.on_drop = value)));
	}

	window_focus_callback glfw_window::set_focus_callback(window_focus_callback value)
	{
		return reinterpret_cast<window_focus_callback>(
			glfwSetWindowFocusCallback(m_window,
				reinterpret_cast<GLFWwindowfocusfun>(m_clbk.on_focus = value)));
	}

	window_framebuffer_resize_callback glfw_window::set_framebuffer_resize_callback(window_framebuffer_resize_callback value)
	{
		return reinterpret_cast<window_framebuffer_resize_callback>(
			glfwSetFramebufferSizeCallback(m_window,
				reinterpret_cast<GLFWframebuffersizefun>(m_clbk.on_framebuffer_resize = value)));
	}

	window_iconify_callback glfw_window::set_iconify_callback(window_iconify_callback value)
	{
		return reinterpret_cast<window_iconify_callback>(
			glfwSetWindowIconifyCallback(m_window,
				reinterpret_cast<GLFWwindowiconifyfun>(m_clbk.on_iconify = value)));
	}
	
	window_key_callback glfw_window::set_key_callback(window_key_callback value)
	{
		return reinterpret_cast<window_key_callback>(
			glfwSetKeyCallback(m_window,
				reinterpret_cast<GLFWkeyfun>(m_clbk.on_key = value)));
	}

	window_maximize_callback glfw_window::set_maximize_callback(window_maximize_callback value)
	{
		return reinterpret_cast<window_maximize_callback>(
			glfwSetWindowMaximizeCallback(m_window,
				reinterpret_cast<GLFWwindowmaximizefun>(m_clbk.on_maximize = value)));
	}
	
	window_mouse_button_callback glfw_window::set_mouse_button_callback(window_mouse_button_callback value)
	{
		return reinterpret_cast<window_mouse_button_callback>(
			glfwSetMouseButtonCallback(m_window,
				reinterpret_cast<GLFWmousebuttonfun>(m_clbk.on_mouse_button = value)));
	}

	window_mouse_enter_callback glfw_window::set_mouse_enter_callback(window_mouse_enter_callback value)
	{
		return reinterpret_cast<window_mouse_enter_callback>(
			glfwSetCursorEnterCallback(m_window,
				reinterpret_cast<GLFWcursorenterfun>(m_clbk.on_mouse_enter = value)));
	}

	window_mouse_pos_callback glfw_window::set_mouse_pos_callback(window_mouse_pos_callback value)
	{
		return reinterpret_cast<window_mouse_pos_callback>(
			glfwSetCursorPosCallback(m_window,
				reinterpret_cast<GLFWcursorposfun>(m_clbk.on_mouse_pos = value)));
	}
	
	window_position_callback glfw_window::set_position_callback(window_position_callback value)
	{
		return reinterpret_cast<window_position_callback>(
			glfwSetWindowPosCallback(m_window,
				reinterpret_cast<GLFWwindowposfun>(m_clbk.on_position = value)));
	}

	window_refresh_callback glfw_window::set_refresh_callback(window_refresh_callback value)
	{
		return reinterpret_cast<window_refresh_callback>(
			glfwSetWindowRefreshCallback(m_window,
				reinterpret_cast<GLFWwindowrefreshfun>(m_clbk.on_refresh = value)));
	}

	window_resize_callback glfw_window::set_resize_callback(window_resize_callback value)
	{
		return reinterpret_cast<window_resize_callback>(
			glfwSetWindowSizeCallback(m_window,
				reinterpret_cast<GLFWwindowposfun>(m_clbk.on_resize = value)));
	}

	window_scroll_callback glfw_window::set_scroll_callback(window_scroll_callback value)
	{
		return reinterpret_cast<window_scroll_callback>(
			glfwSetScrollCallback(m_window,
				reinterpret_cast<GLFWscrollfun>(m_clbk.on_mouse_wheel = value)));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}