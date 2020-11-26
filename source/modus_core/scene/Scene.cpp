#include <modus_core/scene/Scene.hpp>
#include <modus_core/scene/Entity.hpp>
#include <modus_core/scene/Components.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	scene::scene(event_bus * bus, allocator_type alloc) noexcept
		: event_listener{ bus }
		, m_entities	{ alloc }
		, m_registry	{}
	{
	}

	scene::~scene() noexcept
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ds::ref<entity> scene::new_entity(ds::string const & name, allocator_type alloc) noexcept
	{
		auto & e{ m_entities.emplace_back(alloc_ref<entity>(alloc, this, m_registry.create())) };
		e->add_component<tag_component>(!name.empty() ? name : "New Entity");
		e->add_component<transform_component>();
		return e;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void scene::from_json(json const & j)
	{
	}

	void scene::to_json(json & j) const
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void scene::on_event(event const &)
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}