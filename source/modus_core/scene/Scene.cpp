#include <modus_core/scene/Scene.hpp>
#include <modus_core/scene/Entity.hpp>
#include <modus_core/scene/Components.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	scene::~scene() noexcept
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ds::ref<entity> scene::new_entity(ds::string const & name, allocator_type alloc) noexcept
	{
		auto & temp
		{
			m_entities.emplace_back(alloc_ref<entity>(alloc, this, m_registry.create()))
		};
		
		temp->add_component<tag_component>(!name.empty() ? name : "New Entity");
		
		temp->add_component<transform_component>();
		
		return temp;
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
			return debug::failure("failed reading scene file: \'{0}\'", path);
		}
	}

	bool scene::load_from_memory(json const & j)
	{
		return false;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void scene::on_event(event const &)
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}