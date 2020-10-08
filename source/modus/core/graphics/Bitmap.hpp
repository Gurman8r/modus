#ifndef _ML_BITMAP_HPP_
#define _ML_BITMAP_HPP_

#include <core/Export.hpp>
#include <core/system/Memory.hpp>
#include <core/detail/Color.hpp>
#include <core/detail/Rect.hpp>

namespace ml
{
	struct ML_CORE_API bitmap final : trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type			= typename pmr::polymorphic_allocator<byte_t>;
		using pixels					= typename pmr::vector<byte_t>;
		using iterator					= typename pixels::iterator;
		using const_iterator			= typename pixels::const_iterator;
		using reverse_iterator			= typename pixels::reverse_iterator;
		using const_reverse_iterator	= typename pixels::const_reverse_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bitmap(allocator_type alloc = {}) noexcept
			: m_pix{ alloc }, m_size{}, m_channels{}, m_path{}
		{
		}

		bitmap(size_t w, size_t h, size_t c, allocator_type alloc = {}) noexcept
			: m_pix{ alloc }, m_size{ w, h }, m_channels{ c }, m_path{}
		{
		}

		bitmap(vec2s s, size_t c, allocator_type alloc = {}) noexcept
			: m_pix{ alloc }, m_size{ s }, m_channels{ c }, m_path{}
		{
		}

		bitmap(byte_t * p, size_t w, size_t h, size_t c, allocator_type alloc = {}) noexcept
			: m_pix{ p, p + (w * h * c), alloc }, m_size{ w, h }, m_channels{ c }, m_path{}
		{
		}

		bitmap(byte_t * p, vec2s s, size_t c, allocator_type alloc = {}) noexcept
			: m_pix{ p, p + (s[0] * s[1] * c), alloc }, m_size{ s }, m_channels{ c }, m_path{}
		{
		}

		bitmap(pixels const & p, size_t w, size_t h, size_t c, allocator_type alloc = {}) noexcept
			: m_pix{ p, alloc }, m_size{ w, h }, m_channels{ c }, m_path{}
		{
		}

		bitmap(pixels const & p, vec2s s, size_t c, allocator_type alloc = {}) noexcept
			: m_pix{ p, alloc }, m_size{ s }, m_channels{ c }, m_path{}
		{
		}

		bitmap(bitmap const & other, allocator_type alloc = {})
			: bitmap{ alloc }
		{
			this->copy(other);
		}

		bitmap(bitmap && other, allocator_type alloc = {}) noexcept
			: bitmap{ alloc }
		{
			this->swap(std::move(other));
		}

		bitmap(fs::path const & path, bool flip_v = true, size_t req = 0, allocator_type alloc = {})
			: bitmap{ alloc }
		{
			this->load_from_file(path, flip_v, req);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bitmap & operator=(bitmap const & other)
		{
			this->copy(other);
			return (*this);
		}

		bitmap & operator=(bitmap && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		void copy(bitmap const & other) noexcept
		{
			if (this != std::addressof(other))
			{
				m_pix		= other.m_pix;
				m_size		= other.m_size;
				m_channels	= other.m_channels;
				m_path		= other.m_path;
			}
		}

		void dump(pixels & other) noexcept
		{
			if (std::addressof(m_pix) != std::addressof(other))
			{
				m_pix.swap(other);
			}
		}

		void swap(bitmap & other) noexcept
		{
			if (this != std::addressof(other))
			{
				m_pix.swap(other.m_pix);
				m_size.swap(other.m_size);
				std::swap(m_channels, other.m_channels);
				m_path.swap(other.m_path);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static bool load_from_file(
			fs::path const	&	path,
			pixels			&	pix,
			vec2s			&	size,
			size_t			&	channels,
			bool				flip_v		= true,
			size_t				req			= 0);

		bool load_from_file(fs::path const & path, bool flip_v = true, size_t req = 0)
		{
			return load_from_file((m_path = path), m_pix, m_size, m_channels, flip_v, req);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void flip_vertically()
		{
			auto const columns{ cols() };
			for (size_t y = 0; y < m_size[1]; ++y)
			{
				auto lhs{ begin() + y * columns }, rhs{ begin() + (y + 1) * columns - m_channels };
				for (size_t x = 0; x < m_size[0] / 2; ++x)
				{
					std::swap_ranges(lhs, lhs + m_channels, rhs);
					lhs += m_channels;
					rhs -= m_channels;
				}
			}
		}

		void flip_horizontally()
		{
			auto const columns{ cols() };
			auto top{ begin() }, bot{ end() - columns };
			for (size_t y = 0; y < m_size[1] / 2; ++y)
			{
				std::swap_ranges(top, top + columns, bot);
				top += columns;
				bot -= columns;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD std::optional<color32> get_pixel(size_t index) const noexcept
		{
			return (index >= capacity()) ? std::nullopt : std::make_optional(color32
			{
				(m_channels >= 1) ? *((cbegin() + index) + 0) : (byte_t)0,
				(m_channels >= 2) ? *((cbegin() + index) + 1) : (byte_t)0,
				(m_channels >= 3) ? *((cbegin() + index) + 2) : (byte_t)0,
				(m_channels >= 4) ? *((cbegin() + index) + 3) : (byte_t)0
			});
		}

		ML_NODISCARD std::optional<color32> get_pixel(size_t x, size_t y) const noexcept
		{
			return get_pixel((x + y * m_size[0]) * m_channels);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool set_pixel(size_t index, color32 const & c) noexcept
		{
			if (index >= capacity()) { return false; }
			else
			{
				auto it{ begin() + index };
				if (m_channels >= 1) *it++ = c[0];
				if (m_channels >= 2) *it++ = c[1];
				if (m_channels >= 3) *it++ = c[2];
				if (m_channels >= 4) *it++ = c[3];
				return true;
			}
		}

		bool set_pixel(size_t x, size_t y, color32 const & c) noexcept
		{
			return set_pixel((x + y * m_size[0]) * m_channels, c);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD operator bool() const noexcept { return !m_pix.empty() && capacity(); }

		ML_NODISCARD auto pix() & noexcept -> pixels & { return m_pix; }

		ML_NODISCARD auto pix() const & noexcept -> pixels const & { return m_pix; }

		ML_NODISCARD auto data() noexcept -> byte_t * { return m_pix.data(); }

		ML_NODISCARD auto data() const noexcept -> byte_t const * { return m_pix.data(); }

		ML_NODISCARD auto size() const & noexcept -> vec2s const & { return m_size; }

		ML_NODISCARD auto channels() const noexcept -> size_t { return m_channels; }

		ML_NODISCARD auto path() const & noexcept -> fs::path const & { return m_path; }

		ML_NODISCARD auto width() const noexcept -> size_t { return m_size[0]; }

		ML_NODISCARD auto height() const noexcept -> size_t { return m_size[1]; }

		ML_NODISCARD auto capacity() const noexcept -> size_t { return m_size[0] * m_size[1] * m_channels; }

		ML_NODISCARD auto cols() const noexcept -> size_t { return m_size[0] * m_channels; }

		ML_NODISCARD auto rows() const noexcept -> size_t { return m_size[1] * m_channels; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto operator[](size_t i) & noexcept -> byte_t & { return m_pix[i]; }

		ML_NODISCARD auto operator[](size_t i) const & noexcept -> byte_t const & { return m_pix[i]; }

		ML_NODISCARD auto begin() noexcept -> iterator { return m_pix.begin(); }
		
		ML_NODISCARD auto begin() const noexcept -> const_iterator { return m_pix.begin(); }
		
		ML_NODISCARD auto cbegin() const noexcept -> const_iterator { return m_pix.cbegin(); }

		ML_NODISCARD auto cend() const noexcept -> const_iterator { return m_pix.cend(); }

		ML_NODISCARD auto crbegin() const noexcept -> const_reverse_iterator { return m_pix.crbegin(); }

		ML_NODISCARD auto crend() const noexcept -> const_reverse_iterator { return m_pix.crend(); }

		ML_NODISCARD auto end() noexcept -> iterator { return m_pix.end(); }
		
		ML_NODISCARD auto end() const noexcept -> const_iterator { return m_pix.end(); }

		ML_NODISCARD auto rbegin() noexcept -> reverse_iterator { return m_pix.rbegin(); }
		
		ML_NODISCARD auto rbegin() const noexcept -> const_reverse_iterator { return m_pix.rbegin(); }

		ML_NODISCARD auto rend() noexcept -> reverse_iterator { return m_pix.rend(); }
		
		ML_NODISCARD auto rend() const noexcept -> const_reverse_iterator { return m_pix.rend(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		pixels		m_pix		; // 
		vec2s		m_size		; // 
		size_t		m_channels	; // 
		fs::path	m_path		; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_BITMAP_HPP_