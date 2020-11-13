#ifndef _ML_SCENE_HPP_
#define _ML_SCENE_HPP_

#include <modus_core/detail/Events.hpp>
#include <modus_core/detail/Matrix.hpp>
#include <entt/entt.hpp>

namespace ml
{
	struct entity;

	struct ML_CORE_API scene : non_copyable, trackable, event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		using entity_list = typename ds::list<ds::ref<entity>>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		scene(event_bus * bus, allocator_type alloc = {}) noexcept
			: event_listener{ bus }
			, m_entities	{ alloc }
			, m_registry	{}
		{
		}

		virtual ~scene() noexcept override = default;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ds::ref<entity> new_entity(ds::string const & name = {}, allocator_type alloc = {}) noexcept;

		auto delete_entity(ds::ref<entity> const & value) noexcept -> entity_list::iterator
		{
			if (auto const it{ std::find(m_entities.begin(), m_entities.end(), value) }
			; it == m_entities.end()) { return it; }
			else { return m_entities.erase(it); }
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool load_from_file(fs::path const & path);

		bool load_from_memory(json const & j);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_entities() noexcept -> entity_list & { return m_entities; }

		ML_NODISCARD auto get_registry() noexcept -> entt::registry & { return m_registry; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using event_listener::get_bus;

	protected:
		using event_listener::subscribe;

		using event_listener::unsubscribe;

		virtual void on_event(event const &) override {}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		entity_list		m_entities	; // entities
		entt::registry	m_registry	; // registry

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_SCENE_HPP_