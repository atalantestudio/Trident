#pragma once

namespace ProjectA {
	extern ConsoleLogger engineLogger;
}

#ifdef BUILD_MODE_DEBUG
	#define ENGINE_TRACE(...) ::ProjectA::engineLogger.trace(__FILE__, __LINE__, __VA_ARGS__)
	#define ENGINE_DEBUG(...) ::ProjectA::engineLogger.debug(__VA_ARGS__)
	#define ENGINE_INFO(...) ::ProjectA::engineLogger.info(__VA_ARGS__)
	#define ENGINE_WARNING(...) ::ProjectA::engineLogger.warning(__VA_ARGS__)
	#define ENGINE_ERROR(...) ::ProjectA::engineLogger.error(__func__, __FILE__, __LINE__, __VA_ARGS__)

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
#else
	#define ENGINE_TRACE(...)
	#define ENGINE_DEBUG(...)
	#define ENGINE_INFO(...)
	#define ENGINE_WARNING(...)
	#define ENGINE_ERROR(...)

	#define DEBUG_ASSERT(...)
	#define ENGINE_ASSERT(...)
#endif

// This feature is not yet implemented.
#define ENGINE_ERROR_UNIMPLEMENTED() { \
	ENGINE_ERROR("This feature is not yet implemented."); \
	ABORT(); \
}

// This feature is not supported.
#define ENGINE_ERROR_UNSUPPORTED() { \
	ENGINE_ERROR("This feature is not supported."); \
	ABORT(); \
}