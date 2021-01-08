#include "./Win32.hpp"
#include "./Win32_Platform.hpp"

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool win32_platform::initialize()
	{
		return false;
	}

	bool win32_platform::finalize()
	{
		return false;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	library_handle win32_platform::load_library(fs::path const & path)
	{
		return (library_handle)LoadLibraryExW(path.c_str(), nullptr, 0u);
	}

	bool win32_platform::free_library(library_handle handle)
	{
		return FreeLibrary((HMODULE)handle);
	}

	void * win32_platform::get_proc_address(library_handle handle, string const & method_name)
	{
		return GetProcAddress((HMODULE)handle, method_name.c_str());
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	window_handle win32_platform::get_active_window()
	{
		return (window_handle)GetActiveWindow();
	}

	window_handle win32_platform::set_active_window(window_handle handle)
	{
		return (window_handle)SetActiveWindow((HWND)handle);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void win32_platform::poll_events()
	{
		MSG msg;

		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
		}
	}

	void win32_platform::swap_buffers(window_handle handle)
	{
	}

	void win32_platform::swap_interval(int32 value)
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	std::optional<fs::path> win32_platform::get_open_file_name(window_handle handle, string const & filter)
	{
		OPENFILENAMEA ofn;
		CHAR file[MAX_PATH]{};
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = (HWND)handle;
		ofn.lpstrFile = file;
		ofn.nMaxFile = sizeof(file);
		ofn.lpstrFilter = filter.c_str();
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		return (GetOpenFileNameA(&ofn) == TRUE) ? std::make_optional(ofn.lpstrFile) : std::nullopt;
	}

	std::optional<fs::path> win32_platform::get_save_file_name(window_handle handle, string const & filter)
	{
		OPENFILENAMEA ofn;
		CHAR file[MAX_PATH]{};
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = (HWND)handle;
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