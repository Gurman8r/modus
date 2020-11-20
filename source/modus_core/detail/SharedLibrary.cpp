#include <modus_core/detail/SharedLibrary.hpp>

#if defined(ML_os_windows)
#	include <Windows.h>
#elif defined(ML_os_unix)
#	include <unistd.h>
#	if ML_has_include(<dlfcn.h>)
#		include <dlfcn.h>
#	endif
#else
#	error ""
#endif

#include <modus_core/detail/Array.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool shared_library::open(fs::path const & path)
	{
		if (m_handle || path.empty()) { return false; }
		else
		{
			m_hash = hashof((m_path = format_path(path)).string());

			return (m_handle = (library_handle)std::invoke([&]() noexcept
			{
#if defined(ML_os_windows)
				return ::LoadLibraryExW(m_path.c_str(), nullptr, 0);

#elif defined(ML_os_linux)
				return ::dlopen(path.string().c_str(), RTLD_LOCAL | RTLD_LAZY);

#else
				return nullptr;
#endif
			}));
		}
	}

	bool shared_library::close()
	{
		if (!m_handle) { return false; }
		else
		{
			m_path.clear();
			m_hash = {};
			m_procs.clear();

#if defined(ML_os_windows)
			return ::FreeLibrary((HINSTANCE)m_handle);

#elif defined(ML_os_linux)
			return ::dlclose(m_handle);

#else
			return false;
#endif
		}
	}

	void * shared_library::get_proc(ds::string const & name)
	{
		// not open
		if (!m_handle) { return nullptr; }

		// load procedure
		if (auto const it{ m_procs.find(name) }
		; it != m_procs.end()) { return it->second; }
		else
		{
			return m_procs.insert({ name, std::invoke([&]() noexcept
			{
#if defined(ML_os_windows)
				return ::GetProcAddress((HINSTANCE)m_handle, name.c_str());

#elif defined(ML_os_linux)
				return ::dlsym(m_handle, name.c_str());

#else
				return nullptr;
#endif
			}) }).first->second;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}