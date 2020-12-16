#ifndef _ML_VIEWPORT_HPP_
#define _ML_VIEWPORT_HPP_

// WIP

#include <modus_core/graphics/RenderCommand.hpp>

namespace ml
{
	struct viewport : trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type	= typename pmr::polymorphic_allocator<byte>;
		using framebuffer_spec	= typename gfx::spec<gfx::framebuffer>;
		using framebuffer_ref	= typename ds::ref<gfx::framebuffer>;
		using framebuffer_list	= typename ds::list<framebuffer_ref>;

		viewport(allocator_type alloc = {}) noexcept
			: m_rect{}
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

		ML_NODISCARD auto get_rect() const noexcept -> float_rect const &
		{
			return m_rect;
		}

		ML_NODISCARD auto get_position() const noexcept -> vec2
		{
			return m_rect.position();
		}

		ML_NODISCARD auto get_size() const noexcept -> vec2
		{
			return m_rect.size();
		}

		ML_NODISCARD auto get_center() const noexcept -> vec2
		{
			return m_rect.center();
		}

		ML_NODISCARD auto get_left() const noexcept -> float32
		{
			return m_rect.left();
		}

		ML_NODISCARD auto get_right() const noexcept -> float32
		{
			return m_rect.right();
		}

		ML_NODISCARD auto get_width() const noexcept -> float32
		{
			return m_rect.width();
		}

		ML_NODISCARD auto get_height() const noexcept -> float32
		{
			return m_rect.height();
		}

		ML_NODISCARD auto get_top() const noexcept -> float32
		{
			return m_rect.top();
		}

		ML_NODISCARD auto get_bottom() const noexcept -> float32
		{
			return m_rect.bottom();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void set_rect(float_rect const & value) noexcept
		{
			if (m_rect != value)
			{
				m_rect = value;
			}
		}

		void set_position(vec2 const & value) noexcept
		{
			if (m_rect.position() != value)
			{
				m_rect.position(value);
			}
		}

		void set_size(vec2 const & value) noexcept
		{
			if (m_rect.size() != value)
			{
				m_rect.size(value);
			}
		}

		void set_center(vec2 const & value) noexcept
		{
			if (m_rect.center() != value)
			{
				m_rect.center(value);
			}
		}

		void set_left(float32 value) noexcept
		{
			if (m_rect.left() != value)
			{
				m_rect.left(value);
			}
		}

		void set_right(float32 value) noexcept
		{
			if (m_rect.right() != value)
			{
				m_rect.right(value);
			}
		}

		void set_width(float32 value) noexcept
		{
			if (m_rect.width() != value)
			{
				m_rect.width(value);
			}
		}

		void set_height(float32 value) noexcept
		{
			if (m_rect.height() != value)
			{
				m_rect.height(value);
			}
		}

		void set_top(float32 value) noexcept
		{
			if (m_rect.top() != value)
			{
				m_rect.top(value);
			}
		}

		void set_bottom(float32 value) noexcept
		{
			if (m_rect.bottom() != value)
			{
				m_rect.bottom(value);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		float_rect m_rect; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_VIEWPORT_HPP_