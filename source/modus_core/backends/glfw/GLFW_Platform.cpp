#include "./GLFW.hpp"
#include "./GLFW_Platform.hpp"

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

	window_handle glfw_platform::get_active_window()
	{
		return (window_handle)glfwGetCurrentContext();
	}

	window_handle glfw_platform::set_active_window(window_handle value)
	{
		glfwMakeContextCurrent((GLFWwindow *)value);
		return value;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void glfw_platform::poll_events()
	{
		glfwPollEvents();
	}

	void glfw_platform::swap_buffers(window_handle handle)
	{
		glfwSwapBuffers((GLFWwindow *)handle);
	}

	void glfw_platform::swap_interval(int32 value)
	{
		glfwSwapInterval(value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	cursor_handle glfw_platform::create_custom_cursor(size_t w, size_t h, byte const * p, int32 x, int32 y)
	{
		GLFWimage temp{ (int32)w, (int32)h, (byte *)p };

		return (cursor_handle)glfwCreateCursor(&temp, x, y);
	}

	cursor_handle glfw_platform::create_standard_cursor(int32 shape)
	{
		return (cursor_handle)glfwCreateStandardCursor(std::invoke([shape]() noexcept
		{
			switch (shape)
			{
			default							: return 0;
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
		}));
	}

	void glfw_platform::destroy_cursor(cursor_handle const & handle)
	{
		glfwDestroyCursor((GLFWcursor *)handle);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}