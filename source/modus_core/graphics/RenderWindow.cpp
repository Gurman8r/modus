#include <modus_core/graphics/RenderWindow.hpp>
#include <modus_core/graphics/RenderCommand.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	render_window::render_window(allocator_type alloc) noexcept
		: native_window	{ alloc }
		, m_device		{}
		, m_context		{}
	{
	}

	render_window::render_window(
		ds::string			const & title,
		video_mode			const & vm,
		context_settings	const & cs,
		window_hints_				hints,
		void *						userptr,
		allocator_type				alloc
	) noexcept : render_window{ alloc }
	{
		ML_assert(this->open(title, vm, cs, hints, userptr));
	}

	render_window::~render_window() noexcept
	{
		gfx::destroy_device(m_device.release());
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool render_window::open(
		ds::string			const & title,
		video_mode			const & vm,
		context_settings	const & cs,
		window_hints_				hints,
		void *						userptr
	)
	{
		// check already open
		if (is_open())
		{
			return debug::error("render_window is already open");
		}

		// open render_window
		if (!native_window::open(title, vm, cs, hints, userptr))
		{
			return debug::error("failed opening render_window");
		}

		// create render device
		if (m_device.reset(gfx::make_device(cs.api)); !m_device)
		{
			return debug::error("failed creating device");
		}

		// create render context
		set_render_context(m_device->new_context
		({
			cs.api,
			cs.major,
			cs.minor,
			cs.profile,
			cs.depth_bits,
			cs.stencil_bits,
			cs.multisample,
			cs.srgb_capable
		}));

		// setup states
		get_render_context()->execute
		(
			// alpha state
			gfx::command::set_alpha_state
			({
				true,
				gfx::predicate_greater,
				0.001f
			}),

			// blend state
			gfx::command::set_blend_state
			({
				true,
				colors::white,
				gfx::equation_add,
				gfx::factor_src_alpha,
				gfx::factor_one_minus_src_alpha
			}),

			// cull state
			gfx::command::set_cull_state
			({
				false,
				gfx::facet_back,
				gfx::order_ccw
			}),

			// depth state
			gfx::command::set_depth_state
			({
				true,
				gfx::predicate_less,
				{ 0.f, 1.f }
			}),

			// stencil state
			gfx::command::set_stencil_state
			({
				true,
				gfx::predicate_always,
				0,
				0xffffffff
			})
		);

		return true;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}