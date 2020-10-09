#ifndef _ML_ENTITY_HPP_
#define _ML_ENTITY_HPP_

#include <scene/Scene.hpp>

namespace ml
{
	struct entity : trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		entity(scene * scene, entt::entity handle) noexcept
			: m_scene	{ scene }
			, m_handle	{ handle }
		{
		}

		entity() noexcept : entity{ nullptr, entt::null }
		{
		}

		entity(entity const & other) : entity{ other.m_scene, other.m_handle }
		{
		}

		entity(entity && other) noexcept : entity{}
		{
			this->swap(std::move(other));
		}

		virtual ~entity() noexcept
		{
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

		ML_NODISCARD bool valid() const noexcept
		{
			return ML_check(m_scene)->get_registry().valid(m_handle);
		}

		template <class T, class ... Args
		> decltype(auto) add_component(Args && ... args) noexcept
		{
			return ML_check(m_scene)->get_registry().emplace<T>(m_handle, ML_forward(args)...);
		}

		template <class ... T
		> ML_NODISCARD decltype(auto) get_component() noexcept
		{
			return ML_check(m_scene)->get_registry().get<T...>(m_handle);
		}

		template <class ... T
		> ML_NODISCARD bool has_component() const noexcept
		{
			return ML_check(m_scene)->get_registry().has<T...>(m_handle);
		}

		template <class ... T
		> void remove_component() noexcept
		{
			ML_check(m_scene)->get_registry().remove<T...>(m_handle);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_handle() const noexcept -> entt::entity { return m_handle; }

		ML_NODISCARD auto get_scene() const noexcept -> scene * { return m_scene; }

		ML_NODISCARD auto get_registry() const noexcept -> entt::registry & { return ML_check(m_scene)->get_registry(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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
		> ML_NODISCARD bool operator==(U const & other) const noexcept {
			return this->compare(other) == 0;
		}

		template <class U = entity
		> ML_NODISCARD bool operator!=(U const & other) const noexcept {
			return this->compare(other) != 0;
		}

		template <class U = entity
		> ML_NODISCARD bool operator<(U const & other) const noexcept {
			return this->compare(other) < 0;
		}

		template <class U = entity
		> ML_NODISCARD bool operator>(U const & other) const noexcept {
			return this->compare(other) > 0;
		}

		template <class U = entity
		> ML_NODISCARD bool operator<=(U const & other) const noexcept {
			return this->compare(other) <= 0;
		}

		template <class U = entity
		> ML_NODISCARD bool operator>=(U const & other) const noexcept {
			return this->compare(other) >= 0;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		scene *			m_scene		; // scene
		entt::entity	m_handle	; // handle

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_ENTITY_HPP_