#include <modus_core/scene/Scene.hpp>
#include <modus_core/scene/Entity.hpp>
#include <modus_core/scene/Components.hpp>
#include <modus_core/events/SceneEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	entity scene::new_entity(ds::string const & name) noexcept
	{
		entity e{ this, m_registry.create() };
		e.add_component<tag_component>(!name.empty() ? name : "New Entity");
		e.add_component<transform_component>();
		return e;
	}

	void scene::delete_entity(entity const & value) noexcept
	{
		m_registry.destroy(value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void scene::on_runtime_update()
	{
	}

	void scene::on_editor_update()
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void scene::from_json(json const & j)
	{
	}

	void scene::to_json(json & j) const
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> void scene::on_component_added(entity e, tag_component & c)
	{

	}

	template <> void scene::on_component_added(entity e, transform_component & c)
	{

	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}