#include "./Win32.hpp"
#include "./Win32_PlatformAPI.hpp"

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	std::optional<fs::path> win32_platform_api::get_open_file_name(window_handle window, cstring filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[MAX_PATH]{};
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = (HWND)window;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		return (GetOpenFileNameA(&ofn) == TRUE) ? std::make_optional(ofn.lpstrFile) : std::nullopt;
	}

	std::optional<fs::path> win32_platform_api::get_save_file_name(window_handle window, cstring filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[MAX_PATH]{};
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = (HWND)window;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		ofn.lpstrDefExt = std::strchr(filter, '\0') + 1; // extract default extension from filter
		return (GetSaveFileNameA(&ofn) == TRUE) ? std::make_optional(ofn.lpstrFile) : std::nullopt;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}