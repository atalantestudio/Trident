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

	#define ADAPTER_TRACE(...) ::ProjectA::GraphicsAdapter::logger.trace(__FILE__, __LINE__, __VA_ARGS__)
	#define ADAPTER_DEBUG(...) ::ProjectA::GraphicsAdapter::logger.debug(__VA_ARGS__)
	#define ADAPTER_INFO(...) ::ProjectA::GraphicsAdapter::logger.info(__VA_ARGS__)
	#define ADAPTER_WARNING(...) ::ProjectA::GraphicsAdapter::logger.warning(__VA_ARGS__)
	#define ADAPTER_ERROR(...) ::ProjectA::GraphicsAdapter::logger.error(__func__, __FILE__, __LINE__, __VA_ARGS__)

	#define ADAPTER_ASSERT(condition, ...) { \
		if (!(condition)) { \
			ADAPTER_ERROR(__VA_ARGS__); \
			BREAKPOINT(); \
		} \
	}
#else
	#define ENGINE_TRACE(...)
	#define ENGINE_DEBUG(...)
	#define ENGINE_INFO(...)
	#define ENGINE_WARNING(...)
	#define ENGINE_ERROR(...)

	#define ADAPTER_TRACE(...)
	#define ADAPTER_DEBUG(...)
	#define ADAPTER_INFO(...)
	#define ADAPTER_WARNING(...)
	#define ADAPTER_ERROR(...)

	#define DEBUG_ASSERT(...)
	#define ENGINE_ASSERT(...)
	#define ADAPTER_ASSERT(...)
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