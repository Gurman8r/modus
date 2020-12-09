#ifndef _ML_VIEWPORT_HPP_
#define _ML_VIEWPORT_HPP_

// WIP

#include <modus_core/detail/Rect.hpp>
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
			: m_bounds	{}
			, m_fb		{}
		{
		}

		viewport(viewport const & other, allocator_type alloc = {})
			: m_bounds	{ other.m_bounds }
			, m_fb		{ other.m_fb }
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
				m_bounds.swap(other.m_bounds);
				m_fb.swap(other.m_fb);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void recalculate()
		{
			ML_assert((0.f != m_bounds[2]) && (0.f != m_bounds[3]));

			if (m_fb) { m_fb->resize(m_bounds.size()); }
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_framebuffer() const noexcept -> framebuffer_ref const &
		{
			return m_fb;
		}

		void set_framebuffer(framebuffer_ref const & value) noexcept
		{
			if (m_fb != value)
			{
				m_fb = value;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_bounds() const noexcept -> float_rect const &
		{
			return m_bounds;
		}

		ML_NODISCARD auto get_position() const noexcept -> vec2
		{
			return m_bounds.position();
		}

		ML_NODISCARD auto get_resolution() const noexcept -> vec2
		{
			return m_bounds.size();
		}

		ML_NODISCARD auto get_center() const noexcept -> vec2
		{
			return m_bounds.center();
		}

		ML_NODISCARD auto get_left() const noexcept -> float32
		{
			return m_bounds.left();
		}

		ML_NODISCARD auto get_right() const noexcept -> float32
		{
			return m_bounds.right();
		}

		ML_NODISCARD auto get_width() const noexcept -> float32
		{
			return m_bounds.width();
		}

		ML_NODISCARD auto get_height() const noexcept -> float32
		{
			return m_bounds.height();
		}

		ML_NODISCARD auto get_top() const noexcept -> float32
		{
			return m_bounds.top();
		}

		ML_NODISCARD auto get_bottom() const noexcept -> float32
		{
			return m_bounds.bottom();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void set_bounds(float_rect const & value) noexcept
		{
			if (m_bounds != value)
			{
				m_bounds = value;
			}
		}

		void set_position(vec2 const & value) noexcept
		{
			if (m_bounds.position() != value)
			{
				m_bounds.position(value);
			}
		}

		void set_resolution(vec2 const & value) noexcept
		{
			if (m_bounds.size() != value)
			{
				m_bounds.size(value);
			}
		}

		void set_center(vec2 const & value) noexcept
		{
			if (m_bounds.center() != value)
			{
				m_bounds.center(value);
			}
		}

		void set_left(float32 value) noexcept
		{
			if (m_bounds.left() != value)
			{
				m_bounds.left(value);
			}
		}

		void set_right(float32 value) noexcept
		{
			if (m_bounds.right() != value)
			{
				m_bounds.right(value);
			}
		}

		void set_width(float32 value) noexcept
		{
			if (m_bounds.width() != value)
			{
				m_bounds.width(value);
			}
		}

		void set_height(float32 value) noexcept
		{
			if (m_bounds.height() != value)
			{
				m_bounds.height(value);
			}
		}

		void set_top(float32 value) noexcept
		{
			if (m_bounds.top() != value)
			{
				m_bounds.top(value);
			}
		}

		void set_bottom(float32 value) noexcept
		{
			if (m_bounds.bottom() != value)
			{
				m_bounds.bottom(value);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		framebuffer_ref	m_fb		; // 
		float_rect		m_bounds	; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_VIEWPORT_HPP_