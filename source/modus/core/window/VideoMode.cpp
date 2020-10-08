#include <core/window/VideoMode.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if defined(ML_os_windows)
#	include <Windows.h>

#else
#	error ""
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	video_mode const & video_mode::get_desktop_mode()
	{
		static auto temp{ std::invoke([&temp = video_mode{}]()
		{
#ifdef ML_os_windows
			DEVMODE dm; dm.dmSize = sizeof(dm);
			::EnumDisplaySettingsW(nullptr, ENUM_CURRENT_SETTINGS, &dm);
			temp =
			{
				{ (int32_t)dm.dmPelsWidth, (int32_t)dm.dmPelsHeight },
				vec4b::fill((byte_t)dm.dmBitsPerPel),
				(int32_t)dm.dmDisplayFrequency
			};
#else
#	error ""
#endif
			return temp;
		}) };
		return temp;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	pmr::vector<video_mode> const & video_mode::get_fullscreen_modes()
	{
		static auto temp{ std::invoke([&temp = ds::set<video_mode>{}]()
		{
#ifdef ML_os_windows
			DEVMODE dm; dm.dmSize = sizeof(dm);
			for (int32_t i = 0; ::EnumDisplaySettingsW(nullptr, i, &dm); ++i)
			{
				temp.insert
				({
					{ (int32_t)dm.dmPelsWidth, (int32_t)dm.dmPelsHeight },
					vec4b::fill((byte_t)dm.dmBitsPerPel),
					(int32_t)dm.dmDisplayFrequency
				});
			}
#else
#	error ""
#endif
			return temp;
		}) };
		return temp.elements();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}