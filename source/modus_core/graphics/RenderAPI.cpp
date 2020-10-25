#include <modus_core/graphics/RenderAPI.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef ML_IMPL_RENDERER_OPENGL
#include "../platform/opengl/OpenGL_RenderAPI.hpp"
#endif

#ifdef ML_IMPL_RENDERER_DIRECTX
#include "../platform/directx/DirectX_RenderAPI.hpp"
#endif

#ifdef ML_IMPL_RENDERER_VULKAN
#include "../platform/vulkan/Vulkan_RenderAPI.hpp"
#endif

// etc...

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	static render_device * g_device{}; // singleton

	render_device * get_default_device() noexcept {
		return g_device;
	}

	render_device * set_default_device(render_device * value) noexcept {
		return g_device = value;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	render_device * render_device::create(int32_t api, allocator_type alloc) noexcept
	{
		render_device * temp{ std::invoke([&]() noexcept -> render_device *
		{
			switch (api)
			{
			default					: return nullptr;
			case context_api_unknown: return nullptr;
			case context_api_opengl	: return new opengl_render_device{ alloc };
			case context_api_vulkan	: return nullptr;
			case context_api_directx: return nullptr;
			}
		}) };

		if (!g_device) { set_default_device(temp); }

		return temp;
	}

	void render_device::destroy(render_device * value) noexcept
	{
		if (!value) { value = g_device; }

		if (g_device == value) { set_default_device(nullptr); }

		delete value;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}