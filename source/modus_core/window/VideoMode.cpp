#include <modus_core/window/VideoMode.hpp>

#if defined(ML_os_windows)
#	include <Windows.h>
#else
#	error ""
#endif

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	video_mode video_mode::desktop_mode = std::invoke([&temp = video_mode{}]()
	{
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
	});

	list<video_mode> video_mode::fullscreen_modes = std::invoke([&temp = list<video_mode>{}]()
	{
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
	});

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}