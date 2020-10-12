#ifndef _ML_RENDERER_HPP_
#define _ML_RENDERER_HPP_

// WIP

#include <modus_core/graphics/RenderCommand.hpp>

namespace ml
{
	struct render_queue
	{
	};

	struct ML_CORE_API renderer : trackable, non_copyable
	{
		virtual ~renderer() noexcept override = default;
	};

	struct ML_CORE_API renderer2d final : renderer
	{
	};

	struct ML_CORE_API renderer3d final : renderer
	{
	};
}

#endif // !_ML_RENDERER_HPP_