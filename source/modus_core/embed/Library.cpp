#include <modus_core/embed/Library.hpp>

#ifdef ML_os_windows
#include <modus_core/backends/win32/Win32_Platform.hpp>
using impl_platform = _ML win32_platform;
#endif

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool native_library::open(fs::path path)
	{
		if (m_handle || path.empty()) { return false; }

		m_uuid = (library_id)hashof(path.string());

		if (path.extension().empty()) { path += default_extension; }

		if (auto const info{ util::get_file_info(path) }) { m_info = *info; }

		m_handle = impl_platform::load_library(path);

		return (bool)m_handle;
	}

	bool native_library::close()
	{
		if (!m_handle) { return false; }
		m_uuid = 0;
		m_info = {};
		m_procs.clear();
		return impl_platform::free_library(m_handle);
	}

	void * native_library::get_proc_address(string const & method_name)
	{
		if (!m_handle) { return nullptr; }
		else if (auto const it{ m_procs.find(method_name) }; it != m_procs.end()) { return it->second; }
		else { return m_procs.insert({ method_name, impl_platform::get_proc_address(m_handle, method_name) }).first->second; }
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}