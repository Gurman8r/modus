#ifndef _COMPONENTS_HPP_
#define _COMPONENTS_HPP_

// WIP

#include <modus_core/graphics/Camera.hpp>
#include <modus_core/scene/Entity.hpp>

namespace ml
{
	struct ML_NODISCARD tag_component final
	{
		ds::string tag	; // 
	};

	struct ML_NODISCARD transform_component final
	{
		vec3 translation; // 
		vec4 rotation	; // 
		vec3 scale		; // 

		ML_NODISCARD mat4 get_transform() const
		{
			return util::glm_cast(
				glm::translate(glm::mat4{ 1.f }, (glm::vec3)translation) *
				glm::toMat4(glm::quat{ (glm::vec3)rotation }) *
				glm::scale(glm::mat4{ 1.f }, (glm::vec3)scale));
		}
	};

	struct ML_NODISCARD camera_component final
	{
	};
}

#endif // !_COMPONENTS_HPP_