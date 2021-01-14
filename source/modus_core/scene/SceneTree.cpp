#include <modus_core/scene/Components.hpp>
#include <modus_core/events/SceneEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	scene_tree::~scene_tree() noexcept {}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void scene_tree::on_runtime_update(duration dt)
	{
		if (!m_root) { return; }

		m_reg.view<behavior_component>().each([&](auto e, behavior_component & scr)
		{
			if (!scr.instance)
			{
				scr.instance = scr.create_instance();

				scr.instance->on_create();
			}

			scr.instance->on_update(dt);
		});
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> void scene_tree::on_component_added<tag_component>(entity & e, tag_component & c)
	{
	}

	template <> void scene_tree::on_component_added<transform_component>(entity & e, transform_component & c)
	{
	}

	template <> void scene_tree::on_component_added<camera_component>(entity & e, camera_component & c)
	{
	}

	template <> void scene_tree::on_component_added<behavior_component>(entity & e, behavior_component & c)
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}