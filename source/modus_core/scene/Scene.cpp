#include <modus_core/scene/Scene.hpp>
#include <modus_core/scene/Entity.hpp>
#include <modus_core/scene/Components.hpp>
#include <modus_core/events/SceneEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	scene_tree::scene_tree(allocator_type alloc) noexcept
		: m_reg	{}
		, m_root{ ML_new(entity, this, "root", nullptr, alloc) }
	{
	}

	scene_tree::~scene_tree() noexcept
	{
		ML_delete(m_root);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	entity * scene_tree::new_entity(string const & name) noexcept
	{
		entity * e{ m_root->new_child(name) };
		e->add_component<tag_component>(!name.empty() ? name : "New Entity");
		e->add_component<transform_component>();
		return e;
	}

	void scene_tree::delete_entity(entity * value) noexcept
	{
		if (!value) { return; }

		m_reg.destroy(*value);

		if (value->m_parent) { value->m_parent->delete_child(value); }
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

	template <> void scene_tree::on_component_added<native_script_component>(entity & e, native_script_component & c)
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}