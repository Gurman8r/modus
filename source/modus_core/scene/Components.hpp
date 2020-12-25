#ifndef _COMPONENTS_HPP_
#define _COMPONENTS_HPP_

// WIP

#include <modus_core/scene/ScriptableEntity.hpp>
#include <modus_core/graphics/Camera.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct ML_NODISCARD tag_component final
	{
		ds::string tag	; // 
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct ML_NODISCARD transform_component final
	{
		vec3 position	; // 
		vec4 rotation	; // 
		vec3 scale		; // 

		ML_NODISCARD mat4 get_transform() const
		{
			return util::glm_cast(
				glm::translate(glm::mat4{ 1.f }, (glm::vec3)position) *
				glm::toMat4(glm::quat{ (glm::vec3)rotation }) *
				glm::scale(glm::mat4{ 1.f }, (glm::vec3)scale));
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct ML_NODISCARD camera_component final
	{
		camera instance;
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct ML_NODISCARD native_script_component final
	{
		scriptable_entity *								instance	; // 
		ds::method<void()>								allocate	; // 
		ds::method<void()>								deallocate	; // 
		ds::method<void(scriptable_entity *)>			on_create	; // 
		ds::method<void(scriptable_entity *)>			on_destroy	; // 
		ds::method<void(scriptable_entity *, duration)>	on_update	; // 

		template <class T> void bind()
		{
			allocate	= [&instance]() { instance = ML_get_global(memory_manager)->new_object<T>(); };
			deallocate	= [&instance]() { ML_get_global(memory_manager)->delete_object((T *)instance); };
			on_create	= [](scriptable_entity * instance) { ((T *)instance)->on_create(); };
			on_destroy	= [](scriptable_entity * instance) { ((T *)instance)->on_destroy(); };
			on_update	= [](scriptable_entity * instance, duration dt) { ((T *)instance)->on_update(std::move(dt)); };
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_COMPONENTS_HPP_