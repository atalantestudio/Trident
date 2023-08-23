#pragma once

#ifdef WINDOWS
	#ifdef BUILD_DLL
		#define API __declspec(dllexport)
	#else
		#define API __declspec(dllimport)
	#endif
#else
	#error Operating system not supported.
#endif

#define BIT_SHIFT_ID(id) (1 << id)