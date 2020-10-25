#ifndef _ML_FONT_HPP_
#define _ML_FONT_HPP_

#include <modus_core/graphics/RenderAPI.hpp>

// glyph
namespace ml
{
	struct ML_CORE_API glyph final : trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		shared<gfx::texture2d> graphic{};

		float_rect bounds{};
		
		uint32_t advance{};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		operator bool() const noexcept { return (bool)graphic; }

		auto bearing() const noexcept -> vec2 { return bounds.position(); }
		
		auto size() const noexcept -> vec2 { return bounds.size(); }
		
		auto left() const noexcept -> float_t { return bearing()[0]; }
		
		auto top() const noexcept -> float_t { return bearing()[1]; }
		
		auto width() const noexcept -> float_t { return size()[0]; }
		
		auto height() const noexcept -> float_t { return size()[1]; }
		
		auto offset() const noexcept -> vec2 { return { left(), -top() }; }
		
		auto step() const noexcept -> float_t { return (float_t)(advance >> 6); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// font
namespace ml
{
	ML_decl_handle(	font_face		); // font face handle
	ML_decl_handle(	font_library	); // font library handle
	ML_decl_handle(	font_stroker	); // font stroker handle

	struct ML_CORE_API font final : trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type	= typename pmr::polymorphic_allocator<byte_t>;
		using page				= typename ds::map<uint32_t, glyph>;
		using page_table		= typename ds::map<uint32_t, page>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		font(allocator_type alloc = {}) noexcept
			: m_pages	{ alloc }
			, m_family	{ alloc }
			, m_library	{}
			, m_face	{}
			, m_stroker	{}
		{
		}

		font(fs::path const & path, allocator_type alloc = {}) : font{ alloc }
		{
			load_from_file(path);
		}

		font(font const & value, allocator_type alloc = {})
			: m_pages	{ value.m_pages, alloc }
			, m_family	{ value.m_family, alloc }
			, m_library	{ value.m_library }
			, m_face	{ value.m_face }
			, m_stroker	{ value.m_stroker }
		{
		}

		font(font && value, allocator_type alloc = {}) noexcept : font{ alloc }
		{
			swap(std::move(value));
		}
		
		~font() noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		font & operator=(font const & value)
		{
			font temp{ value };
			this->swap(temp);
			return (*this);
		}

		font & operator=(font && value) noexcept
		{
			this->swap(std::move(value));
			return (*this);
		}

		void swap(font & value) noexcept
		{
			if (this != std::addressof(value))
			{
				std::swap(m_library	, value.m_library);
				std::swap(m_face	, value.m_face);
				std::swap(m_stroker	, value.m_stroker);
				
				m_family.swap(value.m_family);
				m_pages.swap(value.m_pages);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool load_from_file(fs::path const & path);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD glyph load_glyph(uint32_t c, uint32_t size);

		ML_NODISCARD glyph & get_glyph(uint32_t c, uint32_t size) noexcept
		{
			return m_pages[size].find_or_add_fn(c, &font::load_glyph, this, c, size);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto family() const noexcept -> ds::string const & { return m_family; }

		ML_NODISCARD auto pages() const noexcept -> page_table const & { return m_pages; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		font_library	m_library;
		font_face		m_face;
		font_stroker	m_stroker;
		ds::string		m_family;
		page_table		m_pages;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_FONT_HPP_