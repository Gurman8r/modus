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

	render_device * render_device::g_dev{};

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

		if (!g_dev) { set_default(temp); }

		return temp;
	}

	void render_device::destroy(render_device * value) noexcept
	{
		if (!value) { value = g_dev; }

		if (g_dev == value) { set_default(nullptr); }

		delete value;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}