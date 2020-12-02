#ifndef _ML_SCENE_RENDERER_HPP_
#define _ML_SCENE_RENDERER_HPP_

#include <modus_core/scene/Entity.hpp>
#include <modus_core/graphics/Camera.hpp>
#include <modus_core/graphics/Viewport.hpp>

namespace ml
{
	struct ML_CORE_API scene_renderer
	{
		scene_renderer();

	private:
		viewport m_view;
	};
}

#endif // !_ML_SCENE_RENDERER_HPP_