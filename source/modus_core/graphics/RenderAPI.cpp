#include <modus_core/graphics/RenderAPI.hpp>

#ifdef ML_IMPL_RENDERER_OPENGL
#include <modus_core/backends/opengl/OpenGL_RenderAPI.hpp>
#endif

#ifdef ML_IMPL_RENDERER_DIRECTX
#include <modus_core/backends/directx/DirectX_RenderAPI.hpp>
#endif

#ifdef ML_IMPL_RENDERER_VULKAN
#include <modus_core/backends/vulkan/Vulkan_RenderAPI.hpp>
#endif

namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	render_device * render_device::create(spec_type const & desc, allocator_type alloc)
	{
		auto const temp = std::invoke([&]() -> render_device *
		{
			switch (desc.api)
			{
			default: return nullptr;
			
			case context_api_opengl:
				return ::new (alloc.allocate(sizeof(opengl_render_device)))
					opengl_render_device{ desc, alloc };
			
			case context_api_vulkan:
				return nullptr;
			
			case context_api_directx:
				return nullptr;
			}
		});

		(void)ML_begin_global(render_device, temp);

		return temp;
	}

	void render_device::destroy(render_device * value)
	{
		if (!value) { value = ML_get_global(render_device); }

		(void)ML_end_global(render_device, value);

		delete value;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// global render device
namespace ml::globals
{
	static gfx::render_device * g_render_device{};

	ML_impl_global(gfx::render_device) get_global() noexcept
	{
		return g_render_device;
	}

	ML_impl_global(gfx::render_device) set_global(gfx::render_device * value) noexcept
	{
		return g_render_device = value;
	}
}