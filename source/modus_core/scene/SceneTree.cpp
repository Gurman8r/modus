#include <modus_core/scene/SceneTree.hpp>
#include <modus_core/scene/Components.hpp>
#include <modus_core/events/SceneEvents.hpp>

namespace ml
{
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

	template <> void scene_tree::on_component_added<native_script_component>(entity & e, native_script_component & c)
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}