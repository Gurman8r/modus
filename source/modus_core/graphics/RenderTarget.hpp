#ifndef _ML_RENDER_TARGET_HPP_
#define _ML_RENDER_TARGET_HPP_

// WIP

#include <modus_core/graphics/RenderCommand.hpp>
#include <modus_core/graphics/Viewport.hpp>

namespace ml::gfx
{
	struct render_target : trackable
	{
	public:
		using allocator_type	= typename pmr::polymorphic_allocator<byte>;
		using framebuffer_spec	= typename spec<framebuffer>;
		using framebuffer_ref	= typename ref<framebuffer>;
		using framebuffer_list	= typename list<framebuffer_ref>;
	};
}

#endif // !_ML_RENDER_TARGET_HPP_