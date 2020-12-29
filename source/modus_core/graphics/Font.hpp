#ifndef _ML_FONT_HPP_
#define _ML_FONT_HPP_

#include <modus_core/graphics/RenderAPI.hpp>

// GLYPH
namespace ml
{
	// glyph
	struct ML_CORE_API glyph final : trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ref<gfx::texture2d> graphic{};

		float_rect bounds{};
		
		uint32 advance{};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD operator bool() const noexcept { return (bool)graphic; }

		ML_NODISCARD auto bearing() const noexcept -> vec2 { return bounds.min(); }
		
		ML_NODISCARD auto size() const noexcept -> vec2 { return bounds.size(); }
		
		ML_NODISCARD auto left() const noexcept -> float32 { return bearing()[0]; }
		
		ML_NODISCARD auto top() const noexcept -> float32 { return bearing()[1]; }
		
		ML_NODISCARD auto width() const noexcept -> float32 { return size()[0]; }
		
		ML_NODISCARD auto height() const noexcept -> float32 { return size()[1]; }
		
		ML_NODISCARD auto offset() const noexcept -> vec2 { return { left(), -top() }; }
		
		ML_NODISCARD auto step() const noexcept -> float32 { return (float32)(advance >> 6); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// FONT
namespace ml
{
	ML_decl_handle(	font_face		); // font face handle
	ML_decl_handle(	font_library	); // font library handle
	ML_decl_handle(	font_stroker	); // font stroker handle

	// font
	struct ML_CORE_API font final : trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type	= typename pmr::polymorphic_allocator<byte>;
		using page				= typename ds::map<uint32, glyph>;
		using page_table		= typename ds::map<uint32, page>;

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

		ML_NODISCARD glyph load_glyph(uint32 c, uint32 size);

		ML_NODISCARD glyph & get_glyph(uint32 c, uint32 size) noexcept
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