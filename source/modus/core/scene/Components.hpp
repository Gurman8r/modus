#ifndef _COMPONENTS_HPP_
#define _COMPONENTS_HPP_

#include <core/scene/SceneCamera.hpp>

namespace ml
{
	struct ML_NODISCARD tag_component final
	{
		pmr::string tag;
	};

	struct ML_NODISCARD transform_component final
	{
		mat4 transform;
	};

	struct ML_NODISCARD camera_component final
	{
		camera camera;
	};
}

#endif // !_COMPONENTS_HPP_