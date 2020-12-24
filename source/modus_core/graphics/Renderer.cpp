#include <modus_core/graphics/Renderer.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	renderer::renderer(allocator_type alloc) noexcept
		: m_device	{}
	{
	}

	renderer::renderer(context_settings const & settings, allocator_type alloc) noexcept
		: renderer{ alloc }
	{
		ML_verify(this->initialize(settings, alloc));
	}

	renderer::~renderer()
	{
		this->finalize();
	}

	bool renderer::initialize(context_settings const & settings, allocator_type alloc)
	{
		if (m_device) {
			return debug::failure("render device already initialized");
		}

		if (m_device.reset(gfx::render_device::create({ settings.api }, alloc)); !m_device) {
			return debug::failure("failed creating render device");
		}

		m_device->set_active_context(m_device->new_context(settings));

		m_device->get_active_context()->execute([](gfx::render_context * ctx) noexcept
		{
			ctx->set_alpha_state({});
			ctx->set_blend_state({});
			ctx->set_cull_state({});
			ctx->set_depth_state({});
			ctx->set_stencil_state({});
		});

		return true;
	}

	void renderer::finalize()
	{
		if (m_device)
		{
			gfx::destroy_device(m_device.release());
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}