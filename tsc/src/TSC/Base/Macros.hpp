#pragma once

#if COMPILER == COMPILER_MSVC
	#define BREAKPOINT() __debugbreak()
#else
	#define BREAKPOINT()
#endif

#define ABORT std::abort

#define DEBUG_ASSERT(condition) { \
	if (!(condition)) { \
		MODULE_ERROR("Debug assertion failed."); \
		BREAKPOINT(); \
	} \
}

#define MODULE_ASSERT(condition, ...) { \
	if (!(condition)) { \
		MODULE_ERROR(__VA_ARGS__); \
		BREAKPOINT(); \
	} \
}