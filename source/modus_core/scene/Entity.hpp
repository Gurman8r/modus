#ifndef _ML_ENTITY_HPP_
#define _ML_ENTITY_HPP_

#include <modus_core/scene/Scene.hpp>

namespace ml
{
	struct entity : trackable, event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual ~entity() noexcept override = default;

		explicit entity(scene * scene, entt::entity handle) noexcept
			: event_listener{ ML_check(scene)->get_bus() }
			, m_scene		{ scene }
			, m_handle		{ handle }
		{
		}

		entity(entity const & other)
			: event_listener{ other.get_bus() }
			, m_scene		{ other.m_scene }
			, m_handle		{ other.m_handle }
		{
		}

		entity(entity && other) noexcept
			: event_listener{ other.get_bus() }
			, m_scene		{}
			, m_handle		{}
		{
			this->swap(std::move(other));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		entity & operator=(entity const & other)
		{
			entity temp{ other };
			this->swap(temp);
			return (*this);
		}

		entity & operator=(entity && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		void swap(entity & other) noexcept
		{
			if (this != std::addressof(other))
			{
				std::swap(m_scene, other.m_scene);
				std::swap(m_handle, other.m_handle);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_handle() const noexcept -> entt::entity
		{
			return m_handle;
		}

		ML_NODISCARD auto get_scene() const noexcept -> scene *
		{
			return m_scene;
		}

		ML_NODISCARD auto get_registry() const noexcept -> entt::registry &
		{
			return ML_check(m_scene)->get_registry();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Component, class ... Args
		> decltype(auto) add_component(Args && ... args) noexcept
		{
			return this->get_registry().emplace<Component>(m_handle, ML_forward(args)...);
		}

		template <class ... Components
		> ML_NODISCARD decltype(auto) get_component() noexcept
		{
			return this->get_registry().get<Components...>(m_handle);
		}

		template <class ... Components
		> ML_NODISCARD bool has_component() const noexcept
		{
			return this->get_registry().has<Components...>(m_handle);
		}

		template <class ... Components
		> void remove_component() noexcept
		{
			this->get_registry().remove<T...>(m_handle);
		}

		ML_NODISCARD bool valid() const noexcept
		{
			return this->get_registry().valid(m_handle);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		virtual void on_event(event const &) override {}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		template <class U = entity
		> ML_NODISCARD auto compare(U const & other) const noexcept
		{
			if constexpr (std::is_same_v<U, entity>)
			{
				if (m_scene != other.m_scene)
				{
					return ML_compare(m_scene, other.m_scene);
				}
				else
				{
					return ML_compare(m_handle, other.m_handle);
				}
			}
		}

		template <class U = entity
		> ML_NODISCARD bool operator==(U const & other) const noexcept { return this->compare(other) == 0; }

		template <class U = entity
		> ML_NODISCARD bool operator!=(U const & other) const noexcept { return this->compare(other) != 0; }

		template <class U = entity
		> ML_NODISCARD bool operator<(U const & other) const noexcept { return this->compare(other) < 0; }

		template <class U = entity
		> ML_NODISCARD bool operator>(U const & other) const noexcept { return this->compare(other) > 0; }

		template <class U = entity
		> ML_NODISCARD bool operator<=(U const & other) const noexcept { return this->compare(other) <= 0; }

		template <class U = entity
		> ML_NODISCARD bool operator>=(U const & other) const noexcept { return this->compare(other) >= 0; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		scene *			m_scene		; // scene
		entt::entity	m_handle	; // handle

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_ENTITY_HPP_