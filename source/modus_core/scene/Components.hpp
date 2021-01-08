#ifndef _COMPONENTS_HPP_
#define _COMPONENTS_HPP_

// WIP

#include <modus_core/detail/Timer.hpp>
#include <modus_core/scene/ScriptableEntity.hpp>
#include <modus_core/graphics/Camera.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct ML_NODISCARD tag_component final
	{
		string tag	; // 
	};

	inline void from_json(json const & j, tag_component & v)
	{
		j["tag"].get_to(v.tag);
	}

	inline void to_json(json & j, tag_component const & v)
	{
		j["tag"] = v.tag;
	}

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

	inline void from_json(json const & j, transform_component & v)
	{
		j["position"].get_to(v.position);
		j["rotation"].get_to(v.rotation);
		j["scale"	].get_to(v.scale);
	}

	inline void to_json(json & j, transform_component const & v)
	{
		j["position"] = v.position;
		j["rotation"] = v.rotation;
		j["scale"	] = v.scale;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct ML_NODISCARD camera_component final
	{
		camera instance;
	};

	inline void from_json(json const & j, camera_component & v)
	{
	}

	inline void to_json(json & j, camera_component const & v)
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct ML_NODISCARD native_script_component final
	{
		scriptable_entity *								instance	; // 
		method<void()>								allocate	; // 
		method<void()>								deallocate	; // 
		method<void(scriptable_entity *)>			on_create	; // 
		method<void(scriptable_entity *)>			on_destroy	; // 
		method<void(scriptable_entity *, duration)>	on_update	; // 

		template <class T> void bind()
		{
			allocate	= [&instance]() { instance = ML_new(T); };
			deallocate	= [&instance]() { ML_delete((T *)instance); };
			on_create	= [](scriptable_entity * instance) { ((T *)instance)->on_create(); };
			on_destroy	= [](scriptable_entity * instance) { ((T *)instance)->on_destroy(); };
			on_update	= [](scriptable_entity * instance, duration dt) { ((T *)instance)->on_update(std::move(dt)); };
		}
	};

	inline void from_json(json const & j, native_script_component & v)
	{
	}

	inline void to_json(json & j, native_script_component const & v)
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_COMPONENTS_HPP_