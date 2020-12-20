#ifndef _ML_PLATFORM_HPP_
#define _ML_PLATFORM_HPP_

#include <modus_core/detail/Timer.hpp>
#include <modus_core/detail/Memory.hpp>

namespace ml
{
	ML_decl_handle(cursor_handle);
	ML_decl_handle(library_handle);
	ML_decl_handle(monitor_handle);
	ML_decl_handle(window_handle);
}

namespace ml
{
	class ML_CORE_API platform final
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static library_handle load_library(fs::path const & path);

		static bool free_library(library_handle instance);

		static void * get_proc_address(library_handle instance, ds::string const & name);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static std::optional<fs::path> get_open_file_name(window_handle window, ds::string const & filter);

		static std::optional<fs::path> get_save_file_name(window_handle window, ds::string const & filter);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_PLATFORM_HPP_