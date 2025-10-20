#pragma once

#if COMPILER == COMPILER_MSVC
	#define BREAKPOINT() __debugbreak()
#else
	#define BREAKPOINT()
#endif

#define ABORT std::abort

#define DEBUG_ASSERT(condition) { \
	if (!(condition)) { \
		ENGINE_ERROR("Debug assertion failed."); \
		BREAKPOINT(); \
	} \
}

#define ENGINE_ASSERT(condition, ...) { \
	if (!(condition)) { \
		ENGINE_ERROR(__VA_ARGS__); \
		BREAKPOINT(); \
	} \
}