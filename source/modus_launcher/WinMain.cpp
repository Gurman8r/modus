/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <modus_core/Config.hpp>

#ifdef ML_os_windows

#include <Windows.h>

extern INT main(INT, CHAR*[]);

INT CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, INT nCmdShow)
{
	return main(__argc, __argv);
}

#endif // ML_os_windows

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */