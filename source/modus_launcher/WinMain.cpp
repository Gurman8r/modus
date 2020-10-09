/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <modus_core/Config.hpp>

#ifdef ML_os_windows

#include <Windows.h>

extern INT main();

INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
	return main();
}

#endif // ML_os_windows

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */