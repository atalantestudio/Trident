#pragma once

#ifndef BREAKPOINT
	#if COMPILER == COMPILER_MSVC
		#define BREAKPOINT() __debugbreak()
	#else
		#define BREAKPOINT()
	#endif
#endif

#ifndef ABORT
	#define ABORT std::abort
#endif

#ifndef DEBUG_ASSERT
	#define DEBUG_ASSERT(condition) { \
		if (!(condition)) { \
			BREAKPOINT(); \
		} \
	}
#endif

#define MODULE_ASSERT(condition, ...) { \
	if (!(condition)) { \
		BREAKPOINT(); \
	} \
}