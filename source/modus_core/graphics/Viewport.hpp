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
			: m_dirty	{}
			, m_bounds	{}
			, m_fb		{ alloc }
		{
		}

		viewport(viewport const & other, allocator_type alloc = {})
			: m_dirty	{ other.m_dirty }
			, m_bounds	{ other.m_bounds }
			, m_fb		{ other.m_fb, alloc }
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
				std::swap(m_dirty		, other.m_dirty);
				std::swap(m_bounds		, other.m_bounds);
				std::swap(m_fb			, other.m_fb);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool is_dirty() const noexcept { return m_dirty; }

		bool mark_dirty(bool value) noexcept { return value && (m_dirty |= value); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_allocator() const noexcept -> allocator_type
		{
			return {};
		}

		ML_NODISCARD auto get_aspect() const noexcept -> float32
		{
			return m_bounds.width() / m_bounds.height();
		}

		ML_NODISCARD auto get_bounds() const noexcept -> float_rect const &
		{
			return m_bounds;
		}

		ML_NODISCARD auto get_position() const noexcept -> vec2
		{
			return m_bounds.position();
		}

		ML_NODISCARD auto get_size() const noexcept -> vec2
		{
			return m_bounds.size();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void set_position(vec2 const & value) noexcept
		{
			if (mark_dirty(m_bounds.position() != value))
			{
				m_bounds.position(value);
			}
		}

		void set_size(vec2 const & value) noexcept
		{
			if (mark_dirty(m_bounds.size() != value))
			{
				m_bounds.size(value);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_framebuffers() const noexcept -> framebuffer_list const &
		{
			return m_fb;
		}

		ML_NODISCARD auto get_framebuffer(size_t i) const noexcept -> framebuffer_ref const &
		{
			return m_fb[i];
		}

		auto add_framebuffer(framebuffer_ref const & value) noexcept -> framebuffer_ref
		{
			if (auto const it{ std::find(m_fb.begin(), m_fb.end(), value) }
			; it != m_fb.end()) { return *it; }
			else { return m_fb.emplace_back(value); }
		}

		auto delete_framebuffer(framebuffer_ref const & value) -> framebuffer_list::iterator
		{
			if (auto const it{ std::find(m_fb.begin(), m_fb.end(), value) }
			; it == m_fb.end()) { return it; }
			else { return m_fb.erase(it); }
		}

		template <class ... Args
		> auto new_framebuffer(Args && ... args) noexcept -> framebuffer_ref
		{
			return m_fb.emplace_back(gfx::alloc_framebuffer
			(
				get_allocator(), get_size(), ML_forward(args)...
			));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		bool				m_dirty			; // 
		float_rect			m_bounds		; // 
		framebuffer_list	m_fb			; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_VIEWPORT_HPP_