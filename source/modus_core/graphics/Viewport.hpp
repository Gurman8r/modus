#ifndef _ML_VIEWPORT_HPP_
#define _ML_VIEWPORT_HPP_

// WIP

#include <modus_core/detail/Rect.hpp>
#include <modus_core/system/Memory.hpp>

namespace ml
{
	struct viewport : trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		viewport(allocator_type alloc = {}) noexcept
			: m_rect{}
		{
		}

		viewport(float_rect const & rect, allocator_type alloc = {}) noexcept
			: m_rect{ rect }
		{
		}

		viewport(float_rect const & rect, float_rect const & offset, allocator_type alloc = {}) noexcept
			: m_rect{ rect }
		{
		}

		viewport(viewport const & other, allocator_type alloc = {})
			: m_rect{ other.m_rect }
		{
		}

		viewport(viewport && other, allocator_type alloc = {}) noexcept
			: viewport{ alloc }
		{
			this->swap(std::move(other));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		viewport & operator=(viewport const & other)
		{
			viewport temp{ other };
			this->swap(temp);
			return (*this);
		}

		viewport & operator=(viewport && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		void swap(viewport & other) noexcept
		{
			if (this != std::addressof(other))
			{
				m_rect.swap(other.m_rect);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_rect() const noexcept -> float_rect const & { return m_rect; }

		void set_rect(float_rect const & value) noexcept { m_rect = value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		float_rect m_rect; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_VIEWPORT_HPP_