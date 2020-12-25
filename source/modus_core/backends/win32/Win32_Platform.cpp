#include "./Win32.hpp"
#include "./Win32_Platform.hpp"

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	library_handle win32_platform::load_library(fs::path const & path)
	{
		return (library_handle)LoadLibraryExW(path.c_str(), nullptr, 0u);
	}

	bool win32_platform::free_library(library_handle instance)
	{
		return FreeLibrary((HMODULE)instance);
	}

	void * win32_platform::get_proc_address(library_handle instance, ds::string const & name)
	{
		return GetProcAddress((HMODULE)instance, name.c_str());
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	std::optional<fs::path> win32_platform::get_open_file_name(window_handle window, ds::string const & filter)
	{
		OPENFILENAMEA ofn;
		CHAR file[MAX_PATH]{};
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = (HWND)window;
		ofn.lpstrFile = file;
		ofn.nMaxFile = sizeof(file);
		ofn.lpstrFilter = filter.c_str();
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		return (GetOpenFileNameA(&ofn) == TRUE) ? std::make_optional(ofn.lpstrFile) : std::nullopt;
	}

	std::optional<fs::path> win32_platform::get_save_file_name(window_handle window, ds::string const & filter)
	{
		OPENFILENAMEA ofn;
		CHAR file[MAX_PATH]{};
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = (HWND)window;
		ofn.lpstrFile = file;
		ofn.nMaxFile = sizeof(file);
		ofn.lpstrFilter = filter.c_str();
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		ofn.lpstrDefExt = std::strchr(filter.c_str(), '\0') + 1; // extract default extension from filter
		return (GetSaveFileNameA(&ofn) == TRUE) ? std::make_optional(ofn.lpstrFile) : std::nullopt;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}