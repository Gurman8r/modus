#ifndef _ML_WIN32_WINDOW_HPP_
#define _ML_WIN32_WINDOW_HPP_

// NYI

#include <modus_core/window/BaseWindow.hpp>

static_assert(ML_os_windows);

namespace ml
{
	struct win32_window final
	{
		~win32_window();
	};
}

#endif // !_ML_WIN32_WINDOW_HPP_