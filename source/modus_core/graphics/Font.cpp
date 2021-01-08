#include <modus_core/graphics/Font.hpp>
#include <modus_core/graphics/RenderAPI.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_BITMAP_H
#include FT_STROKER_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	font::~font() noexcept
	{
		if (m_stroker) { FT_Stroker_Done((FT_Stroker)m_stroker); }
		if (m_face) { FT_Done_Face((FT_Face)m_face); }
		if (m_library) { FT_Done_FreeType((FT_Library)m_library); }
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool font::load_from_file(fs::path const & path)
	{
		if (m_library) { return false; }

		auto failure = [&]() {
			if (m_stroker)	{ FT_Stroker_Done((FT_Stroker)m_stroker); }
			if (m_face)		{ FT_Done_Face((FT_Face)m_face); }
			if (m_library)	{ FT_Done_FreeType((FT_Library)m_library); }
			return false;
		};

		if (FT_Init_FreeType((FT_Library *)&m_library)) {
			return failure();
		}

		if (FT_New_Face((FT_Library)m_library, path.string().c_str(), 0, (FT_Face *)&m_face)) {
			return failure();
		}

		if (FT_Stroker_New((FT_Library)m_library, (FT_Stroker *)&m_stroker)) {
			return failure();
		}

		if (FT_Select_Charmap((FT_Face)m_face, FT_ENCODING_UNICODE)) {
			return failure();
		}

		m_family = ((FT_Face)m_face)->family_name;

		return true;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	glyph font::load_glyph(uint32 c, uint32 size)
	{
		// output
		glyph g{};

		// face not loaded
		auto const face{ (FT_Face)m_face };
		if (!face)
		{
			debug::warn("font face is not loaded");

			return g;
		}

		// set size loading glyphs as
		FT_Set_Pixel_Sizes(face, 0, size);

		// load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			debug::warn("font failed loading character: \'{0}\'", c);
			
			return g;
		}

		// set advance
		g.advance = (uint32)face->glyph->advance.x;

		// set bounds
		g.bounds = float_rect
		{
			face->glyph->bitmap_left,
			face->glyph->bitmap_top,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows
		};

		// create texture
		g.graphic = gfx::texture2d::create
		({
			(vec2i)g.size(),
			gfx::texture_format{ gfx::format_rgba, gfx::format_red },
			gfx::texture_flags_default,
			(std::isspace(c, {}) || !std::isgraph(c, {})
				? nullptr
				: face->glyph->bitmap.buffer)
		});

		return g;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}