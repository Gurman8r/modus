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
			: m_clear_color	{ colors::black }
			, m_clear_flags	{ gfx::clear_color }
			, m_position	{}
			, m_resolution	{}
			, m_fb			{ alloc }
		{
		}

		viewport(viewport const & other, allocator_type alloc = {})
			: m_clear_color	{ other.m_clear_color }
			, m_clear_flags	{ other.m_clear_flags }
			, m_position	{ other.m_position }
			, m_resolution	{ other.m_resolution }
			, m_fb			{ other.m_fb, alloc }
		{
		}

		viewport(viewport && other, allocator_type alloc = {}) noexcept : viewport{ alloc }
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
				m_clear_color.swap(other.m_clear_color);
				std::swap(m_clear_flags, other.m_clear_flags);
				m_position.swap(other.m_position);
				m_resolution.swap(other.m_resolution);
				m_fb.swap(other.m_fb);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_allocator() const noexcept -> allocator_type { return {}; }

		ML_NODISCARD auto get_clear_color() const noexcept -> color const &
		{
			return m_clear_color;
		}

		ML_NODISCARD auto get_clear_flags() const noexcept -> int32
		{
			return m_clear_flags;
		}

		ML_NODISCARD auto get_position() const noexcept -> vec2i const &
		{
			return m_position;
		}

		ML_NODISCARD auto get_resolution() const noexcept -> vec2i const &
		{
			return m_resolution;
		}

		void set_clear_color(color const & value) noexcept
		{
			if (m_clear_color != value)
			{
				m_clear_color = value;
			}
		}

		void set_clear_flags(int32 value) noexcept
		{
			if (m_clear_flags != value)
			{
				m_clear_flags = value;
			}
		}

		void set_position(vec2i const & value) noexcept
		{
			if (m_position != value)
			{
				m_position = value;
			}
		}

		void set_resolution(vec2i const & value) noexcept
		{
			if (m_resolution != value)
			{
				m_resolution = value;
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
				get_allocator(), m_resolution, ML_forward(args)...
			));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		color				m_clear_color	; // 
		int32				m_clear_flags	; // 
		vec2i				m_position		; // 
		vec2i				m_resolution	; // 
		framebuffer_list	m_fb			; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_VIEWPORT_HPP_