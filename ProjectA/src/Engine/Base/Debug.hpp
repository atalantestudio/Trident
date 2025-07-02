#pragma once

#if COMPILER == COMPILER_MSVC
	#define BREAKPOINT __debugbreak
#else
	#define BREAKPOINT
#endif