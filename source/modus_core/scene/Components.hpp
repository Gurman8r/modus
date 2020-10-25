#ifndef _COMPONENTS_HPP_
#define _COMPONENTS_HPP_

#include <modus_core/graphics/Camera.hpp>
#include <modus_core/scene/Entity.hpp>

namespace ml
{
	struct ML_NODISCARD tag_component final
	{
		ds::string tag;
	};

	struct ML_NODISCARD transform_component final
	{
		mat4 transform;
	};

	struct ML_NODISCARD camera_component final
	{
	};
}

#endif // !_COMPONENTS_HPP_