#pragma once

namespace ProjectA {
	extern ConsoleLogger engineLogger;

	#ifdef ENABLE_OPENGL_ADAPTER
		extern ConsoleLogger openGlLogger;
	#endif

	#ifdef ENABLE_VULKAN_ADAPTER
		extern ConsoleLogger vulkanLogger;
	#endif
}

#ifdef BUILD_MODE_DEBUG
	#define ENGINE_TRACE(...) ::ProjectA::engineLogger.trace(__FILE__, __LINE__, __VA_ARGS__)
	#define ENGINE_DEBUG(...) ::ProjectA::engineLogger.debug(__VA_ARGS__)
	#define ENGINE_INFO(...) ::ProjectA::engineLogger.info(__VA_ARGS__)
	#define ENGINE_WARNING(...) ::ProjectA::engineLogger.warning(__VA_ARGS__)
	#define ENGINE_ERROR(...) ::ProjectA::engineLogger.error(__func__, __FILE__, __LINE__, __VA_ARGS__)

	#ifdef ENABLE_OPENGL_ADAPTER
		#define OPENGL_TRACE(...) ::ProjectA::openGlLogger.trace(__FILE__, __LINE__, __VA_ARGS__)
		#define OPENGL_DEBUG(...) ::ProjectA::openGlLogger.debug(__VA_ARGS__)
		#define OPENGL_INFO(...) ::ProjectA::openGlLogger.info(__VA_ARGS__)
		#define OPENGL_WARNING(...) ::ProjectA::openGlLogger.warning(__VA_ARGS__)
		#define OPENGL_ERROR(...) ::ProjectA::openGlLogger.error(__func__, __FILE__, __LINE__, __VA_ARGS__)
	#endif

	#ifdef ENABLE_VULKAN_ADAPTER
		#define VULKAN_TRACE(...) ::ProjectA::vulkanLogger.trace(__FILE__, __LINE__, __VA_ARGS__)
		#define VULKAN_DEBUG(...) ::ProjectA::vulkanLogger.debug(__VA_ARGS__)
		#define VULKAN_INFO(...) ::ProjectA::vulkanLogger.info(__VA_ARGS__)
		#define VULKAN_WARNING(...) ::ProjectA::vulkanLogger.warning(__VA_ARGS__)
		#define VULKAN_ERROR(...) ::ProjectA::vulkanLogger.error(__func__, __FILE__, __LINE__, __VA_ARGS__)
	#endif

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
	#define ENGINE_FATAL(...)

	#ifdef ENABLE_OPENGL_ADAPTER
		#define OPENGL_TRACE(...)
		#define OPENGL_DEBUG(...)
		#define OPENGL_INFO(...)
		#define OPENGL_WARNING(...)
		#define OPENGL_ERROR(...)
	#endif

	#ifdef ENABLE_VULKAN_ADAPTER
		#define VULKAN_TRACE(...)
		#define VULKAN_DEBUG(...)
		#define VULKAN_INFO(...)
		#define VULKAN_WARNING(...)
		#define VULKAN_ERROR(...)
	#endif

	#define DEBUG_ASSERT(...)
	#define ENGINE_ASSERT(...)
#endif

#define ENGINE_ERROR_UNIMPLEMENTED() { \
	ENGINE_ERROR("This feature is not yet implemented."); \
	ABORT(); \
}

#define ENGINE_ERROR_UNSUPPORTED() { \
	ENGINE_ERROR("This feature is not supported."); \
	ABORT(); \
}