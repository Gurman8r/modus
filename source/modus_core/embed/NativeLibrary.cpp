#include <modus_core/embed/NativeLibrary.hpp>

#ifdef ML_os_windows
#include <modus_core/backends/win32/Win32_Platform.hpp>
using impl_platform = _ML win32_platform;
#endif

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool native_library::open(fs::path const & path)
	{
		if (m_handle || path.empty()) { return false; }

		fs::path real_path{ path };
		real_path.replace_extension(default_extension);

		m_hash = hashof(real_path.filename().string());

		if (auto const info{ util::get_file_info(real_path) }) { m_info = *info; }

		m_handle = impl_platform::load_library(real_path);

		return (bool)m_handle;
	}

	bool native_library::close()
	{
		if (!m_handle) { return false; }
		m_hash = {};
		m_info = {};
		m_lookup.clear();
		return impl_platform::free_library(m_handle);
	}

	void * native_library::get_proc_address(string const & method_name)
	{
		if (!m_handle)
		{
			return nullptr;
		}
		else if (auto const it{ m_lookup.find(method_name) }; it != m_lookup.end())
		{
			return it->second;
		}
		else
		{
			return m_lookup.insert(
			{
				method_name, impl_platform::get_proc_address(m_handle, method_name)
			}
			).first->second;
		}
	}


	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}