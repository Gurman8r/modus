#include <modus_core/scene/Scene.hpp>
#include <modus_core/scene/Entity.hpp>
#include <modus_core/scene/Components.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	scene::scene(allocator_type alloc) noexcept
		: m_entities{ alloc }
		, m_registry{}
	{
	}

	scene::~scene() noexcept
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	shared<entity> & scene::create_entity(pmr::string const & name, allocator_type alloc) noexcept
	{
		auto & temp{ m_entities.emplace_back(
			std::allocate_shared<entity>(alloc, this, m_registry.create())
		) };
		temp->add_component<tag_component>(!name.empty() ? name : "New Entity");
		temp->add_component<transform_component>();
		return temp;
	}

	void scene::destroy_entity(shared<entity> const & value) noexcept
	{
		if (auto const it{ std::find(m_entities.begin(), m_entities.end(), value) }
		; it != m_entities.end())
		{
			m_registry.destroy(value->get_handle());

			m_entities.erase(it);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool scene::load_from_file(fs::path const & path)
	{
		std::ifstream f{ path }; ML_defer(&f) { f.close(); };
		if (f)
		{
			return load_from_memory(json::parse(f));
		}
		else
		{
			return debug::error("failed reading scene from file: \'{0}\'", path);
		}
	}

	bool scene::load_from_memory(json const & j)
	{
		return false;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}