#ifndef _ML_SCRIPTABLE_ENTITY_HPP_
#define _ML_SCRIPTABLE_ENTITY_HPP_

#include <modus_core/scene/Entity.hpp>

namespace ml
{
	struct scriptable_entity : trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		virtual ~scriptable_entity() noexcept override = default;

		scriptable_entity() noexcept
			: m_entity{}
		{
		}

		explicit scriptable_entity(scene * parent) noexcept
			: m_entity{ parent }
		{
		}

		explicit scriptable_entity(scene * parent, entt::entity handle) noexcept
			: m_entity{ parent, handle }
		{
		}

		explicit scriptable_entity(entity const & value)
			: m_entity{ value }
		{
		}

		explicit scriptable_entity(entity && value) noexcept
			: m_entity{ std::move(value) }
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD operator entity & () & noexcept { return m_entity; }

		ML_NODISCARD operator entity && () && noexcept { return std::move(m_entity); }

		ML_NODISCARD operator entity const & () const & noexcept { return m_entity; }

		ML_NODISCARD operator entt::entity() const noexcept { return (entt::entity)m_entity; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_entity() const noexcept -> entity const & { return m_entity; }

		ML_NODISCARD auto get_handle() const noexcept -> entt::entity { return m_entity.get_handle(); }

		ML_NODISCARD auto get_registry() const noexcept -> entt::registry * { return m_entity.get_registry(); }

		ML_NODISCARD auto get_scene() const noexcept -> scene * { return m_entity.get_scene(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Component, class ... Args
		> decltype(auto) add_component(Args && ... args) noexcept
		{
			return m_entity.add_component<Component>(ML_forward(args)...);
		}

		template <class ... Components
		> ML_NODISCARD decltype(auto) get_component() noexcept
		{
			return m_entity.get_component<Components...>();
		}

		template <class ... Components
		> ML_NODISCARD bool has_component() const noexcept
		{
			return m_entity.has_component<Components...>();
		}

		template <class ... Components
		> void remove_component() noexcept
		{
			m_entity.remove_component<Components...>();
		}

		ML_NODISCARD bool valid() const noexcept
		{
			return m_entity.valid();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		virtual void on_create() {}

		virtual void on_destroy() {}
		
		virtual void on_update(duration) {}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		entity m_entity;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_SCRIPTABLE_ENTITY_HPP_