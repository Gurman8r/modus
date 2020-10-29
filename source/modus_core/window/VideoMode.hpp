#ifndef _ML_VIDEO_MODE_HPP_
#define _ML_VIDEO_MODE_HPP_

#include <modus_core/Export.hpp>
#include <modus_core/detail/Rect.hpp>
#include <modus_core/detail/Set.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// video mode
	struct ML_NODISCARD ML_CORE_API video_mode final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		vec2i	resolution		{ 640, 480 }	; // 
		vec4b	bits_per_pixel	{ 8, 8, 8, 8 }	; // 
		int32_t	refresh_rate	{ -1 }			; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static video_mode const & get_desktop_mode();

		ML_NODISCARD static pmr::vector<video_mode> const & get_fullscreen_modes();

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr bool operator==(video_mode const & other) const noexcept
		{
			return this == std::addressof(other)
				|| resolution == other.resolution;
		}

		ML_NODISCARD constexpr bool operator<(video_mode const & other) const noexcept
		{
			return this != std::addressof(other)
				&& resolution < other.resolution;
		}

		ML_NODISCARD constexpr bool operator!=(video_mode const & other) const noexcept
		{
			return !(*this == other);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	inline void from_json(json const & j, video_mode & value)
	{
		j.at("resolution"		).get_to(value.resolution);
		j.at("bits_per_pixel"	).get_to(value.bits_per_pixel);
		j.at("refresh_rate"		).get_to(value.refresh_rate);
	}

	inline void to_json(json & j, video_mode const & value)
	{
		j["resolution"		] = value.resolution;
		j["bits_per_pixel"	] = value.bits_per_pixel;
		j["refresh_rate"	] = value.refresh_rate;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_VIDEO_MODE_HPP_