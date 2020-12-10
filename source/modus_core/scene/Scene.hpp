#ifndef _ML_SCENE_HPP_
#define _ML_SCENE_HPP_

#include <modus_core/detail/LoopSystem.hpp>
#include <modus_core/detail/Matrix.hpp>
#include <entt/entt.hpp>

namespace ml
{
	struct entity;

	struct ML_CORE_API scene : event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type			= typename pmr::polymorphic_allocator<byte>;
		using storage_type				= typename ds::list<ds::ref<entity>>;
		using iterator					= typename storage_type::iterator;
		using const_iterator			= typename storage_type::const_iterator;
		using reverse_iterator			= typename storage_type::reverse_iterator;
		using const_reverse_iterator	= typename storage_type::const_reverse_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		scene(event_bus * bus, allocator_type alloc = {}) noexcept;

		virtual ~scene() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD ds::ref<entity> new_entity(ds::string const & name = {}, allocator_type alloc = {}) noexcept;

		auto delete_entity(ds::ref<entity> const & value) noexcept -> iterator
		{
			if (auto const it{ std::find(m_entities.begin(), m_entities.end(), value) }
			; it == m_entities.end()) { return it; }
			else { return m_entities.erase(it); }
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void from_json(json const & j);

		void to_json(json & j) const;

		ML_NODISCARD json to_json() const noexcept { json j{}; this->to_json(j); return j; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool load_from_file(fs::path const & path)
		{
			std::ifstream f{ path }; ML_defer(&f) { f.close(); };
			if (!f) { return debug::failure("failed reading scene: \'{0}\'", path); }
			else
			{
				json j{ json::parse(f) };
				from_json(j);
				return true;
			}
		}

		bool save_to_file(fs::path const & path) const
		{
			std::ofstream f{ path }; ML_defer(&f) { f.close(); };
			if (!f) { return debug::failure("failed writing scene: \'{0}\'", path); }
			else
			{
				f << to_json();
				return true;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_entities() noexcept -> storage_type & { return m_entities; }

		ML_NODISCARD auto get_registry() noexcept -> entt::registry & { return m_registry; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto begin() noexcept -> iterator { return m_entities.begin(); }

		ML_NODISCARD auto begin() const noexcept -> const_iterator { return m_entities.begin(); }

		ML_NODISCARD auto cbegin() const noexcept -> const_iterator { return m_entities.cbegin(); }

		ML_NODISCARD auto end() noexcept -> iterator { return m_entities.end(); }

		ML_NODISCARD auto end() const noexcept -> const_iterator { return m_entities.end(); }

		ML_NODISCARD auto cend() const noexcept -> const_iterator { return m_entities.cend(); }

		ML_NODISCARD auto rbegin() noexcept -> reverse_iterator { return m_entities.rbegin(); }

		ML_NODISCARD auto rbegin() const noexcept -> const_reverse_iterator { return m_entities.rbegin(); }

		ML_NODISCARD auto crbegin() const noexcept -> const_reverse_iterator { return m_entities.crbegin(); }

		ML_NODISCARD auto rend() noexcept -> reverse_iterator { return m_entities.rend(); }

		ML_NODISCARD auto rend() const noexcept -> const_reverse_iterator { return m_entities.rend(); }

		ML_NODISCARD auto crend() const noexcept -> const_reverse_iterator { return m_entities.crend(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		virtual void on_event(event const &) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		storage_type	m_entities	; // entities
		entt::registry	m_registry	; // registry

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

namespace ml
{
	inline void from_json(json const & j, scene & v) noexcept { v.from_json(j); }

	inline void to_json(json & j, scene const & v) noexcept { v.to_json(j); }
}

#endif // !_ML_SCENE_HPP_