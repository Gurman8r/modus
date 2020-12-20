#ifndef _ML_ENTITY_HPP_
#define _ML_ENTITY_HPP_

#include <modus_core/scene/Scene.hpp>

namespace ml
{
	struct entity : trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual ~entity() noexcept override = default;

		explicit entity(scene * scene) noexcept
			: m_scene{ ML_check(scene) }
			, m_handle{ m_scene->m_registry.create() }
		{
		}

		entity(scene * scene, entt::entity handle) noexcept
			: m_scene{ ML_check(scene) }
			, m_handle{ handle }
		{
		}

		entity(entity const & other)
			: m_scene{ other.m_scene }
			, m_handle{ other.m_handle }
		{
		}

		entity(entity && other) noexcept
			: m_scene{}
			, m_handle{}
		{
			this->swap(std::move(other));
		}

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

		ML_NODISCARD operator entt::entity() const noexcept
		{
			return m_handle;
		}

		ML_NODISCARD auto get_handle() const noexcept -> entt::entity
		{
			return m_handle;
		}

		ML_NODISCARD auto get_scene() const noexcept -> scene *
		{
			return m_scene;
		}

		ML_NODISCARD auto registry() const noexcept -> entt::registry &
		{
			return m_scene->registry();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Component, class ... Args
		> auto add_component(Args && ... args) noexcept -> Component &
		{
			auto && c{ m_scene->m_registry.emplace<Component>(m_handle, ML_forward(args)...) };
			m_scene->on_component_added(*this, c);
			return c;
		}

		template <class ... Components
		> ML_NODISCARD decltype(auto) get_component() noexcept
		{
			return m_scene->m_registry.get<Components...>(m_handle);
		}

		template <class ... Components
		> ML_NODISCARD bool has_component() const noexcept
		{
			return m_scene->m_registry.has<Components...>(m_handle);
		}

		template <class ... Components
		> void remove_component() noexcept
		{
			m_scene->m_registry.remove<T...>(m_handle);
		}

		ML_NODISCARD bool valid() const noexcept
		{
			return m_scene->m_registry.valid(m_handle);
		}

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
		friend scene;

		scene * 		m_scene		; // scene
		entt::entity	m_handle	; // handle

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_ENTITY_HPP_