#include <modus_core/window/VideoMode.hpp>

#if defined(ML_os_windows)
#	include <Windows.h>
#else
#	error ""
#endif

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	video_mode const & video_mode::get_desktop_mode()
	{
		static video_mode temp{};

#ifdef ML_os_windows
		DEVMODE dm; dm.dmSize = sizeof(dm);
		::EnumDisplaySettingsW(nullptr, ENUM_CURRENT_SETTINGS, &dm);
		temp =
		{
			{ (int32)dm.dmPelsWidth, (int32)dm.dmPelsHeight },
			vec4b::fill((byte)dm.dmBitsPerPel),
			(int32)dm.dmDisplayFrequency
		};
#else
#	error ""
#endif
		return temp;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ds::list<video_mode> const & video_mode::get_fullscreen_modes()
	{
		static ds::list<video_mode> temp{};

#ifdef ML_os_windows
		DEVMODE dm; dm.dmSize = sizeof(dm);
		for (int32 i = 0; ::EnumDisplaySettingsW(nullptr, i, &dm); ++i)
		{
			temp.push_back
			({
				{ (int32)dm.dmPelsWidth, (int32)dm.dmPelsHeight },
				vec4b::fill((byte)dm.dmBitsPerPel),
				(int32)dm.dmDisplayFrequency
			});
		}
#else
#	error ""
#endif
		return temp;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}