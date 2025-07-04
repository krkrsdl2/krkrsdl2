/* SPDX-License-Identifier: MIT */
/* Copyright (c) Kirikiri SDL2 Developers */

#include "SDLApplication.h"
#include "SysInitImpl.h"
#ifdef USE_SDL_MAIN
#include <SDL_main.h>
#endif

#if defined(USE_SDL_MAIN)
extern "C" int SDL_main(int argc, char **argv)
#elif defined(_WIN32) && defined(_UNICODE)
extern "C" int wmain(int argc, wchar_t **argv)
#else
extern "C" int main(int argc, char **argv)
#endif
{
	try
	{
		krkrsdl2_pre_init_platform();

#if defined(_WIN32) && defined(_UNICODE)
		krkrsdl2_set_args(argc, argv);
#else
		krkrsdl2_convert_set_args(argc, argv);
#endif

		if (krkrsdl2_init_platform())
		{
			TVPTerminateCode = 0;
			return TVPTerminateCode;
		}

		krkrsdl2_run_main_loop();

#ifndef __EMSCRIPTEN__
		krkrsdl2_cleanup();
#endif
	}
	catch (...)
	{
		TVPTerminateCode = 2;
		return TVPTerminateCode;
	}
#ifdef _WIN32
	::TerminateProcess(GetCurrentProcess(), (UINT)TVPTerminateCode);
#endif
	return TVPTerminateCode;
}

