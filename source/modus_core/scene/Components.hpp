#ifndef _COMPONENTS_HPP_
#define _COMPONENTS_HPP_

// WIP

#include <modus_core/detail/Timer.hpp>
#include <modus_core/scene/BehaviorScript.hpp>
#include <modus_core/graphics/Camera.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// tag component
	struct ML_NODISCARD tag_component
	{
		string tag; // 
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

	// transform component
	struct ML_NODISCARD transform_component
	{
		vec3 position; // 
		vec4 rotation; // 
		vec3 scale; // 

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
		j["scale"].get_to(v.scale);
	}

	inline void to_json(json & j, transform_component const & v)
	{
		j["position"] = v.position;
		j["rotation"] = v.rotation;
		j["scale"] = v.scale;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// camera component
	struct ML_NODISCARD camera_component
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

	// behavior component
	struct ML_NODISCARD behavior_component
	{
		behavior_script * instance{};
		method<behavior_script * ()> create_instance{};
		method<void(behavior_component *)> destroy_instance{};

		~behavior_component() noexcept { if (destroy_instance) { destroy_instance(this); } }

		template <class T> void bind()
		{
			create_instance = []()
			{
				return ML_new(T);
			};

			destroy_instance = [](auto scr)
			{
				if (scr && scr->instance)
				{
					ML_delete((T *)scr->instance); scr->instance = nullptr;
				}
			};
		}
	};

	inline void from_json(json const & j, behavior_component & v)
	{
	}

	inline void to_json(json & j, behavior_component const & v)
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_COMPONENTS_HPP_