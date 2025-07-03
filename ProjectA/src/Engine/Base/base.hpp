#pragma once

#include "Engine/config.hpp"

#include "Engine/Base/context.hpp"

#ifdef BUILD_MODE_DEBUG
	#include "Engine/Base/Debug.hpp"
#endif

#include "Engine/Base/types.hpp"

#include "Engine/Base/Limits.hpp"
#include "Engine/Base/binary.hpp"
#include "Engine/Base/cast.hpp"
#include "Engine/Base/enum.hpp"
#include "Engine/Base/function.hpp"

#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
	#include "Engine/Base/Windows.hpp"
#endif

#ifdef BUILD_MODE_DEBUG
	#define DEBUG_ASSERT(condition) { \
		if (!(condition)) { \
			ENGINE_ERROR("Debug assertion failed."); \
			BREAKPOINT(); \
		} \
	}

	#define DEBUG_STATIC_ASSERT static_assert
#else
	#define DEBUG_ASSERT
	#define DEBUG_STATIC_ASSERT
#endif

#include "Engine/Base/Extent.hpp"
#include "Engine/Logger/Logger.hpp"

#define ENGINE_TRACE(argumentOrPattern, ...) ::ProjectA::Logger::engineLogger.trace(argumentOrPattern "\n    at []\n    ([]:[])", ##__VA_ARGS__, __FUNCSIG__, __FILE__, __LINE__)
#define ENGINE_TRACE_PREFORMATTED(...) ::ProjectA::Logger::engineLogger.trace(__VA_ARGS__)
#define ENGINE_DEBUG(...) ::ProjectA::Logger::engineLogger.debug(__VA_ARGS__)
#define ENGINE_INFO(...) ::ProjectA::Logger::engineLogger.info(__VA_ARGS__)
#define ENGINE_WARN(...) ::ProjectA::Logger::engineLogger.warn(__VA_ARGS__)
#define ENGINE_ERROR(...) ::ProjectA::Logger::engineLogger.error(__VA_ARGS__)
#define ENGINE_FATAL(...) ::ProjectA::Logger::engineLogger.fatal(__VA_ARGS__)

#define CLIENT_TRACE(...) ::ProjectA::Logger::clientLogger.trace(__VA_ARGS__)
#define CLIENT_DEBUG(...) ::ProjectA::Logger::clientLogger.debug(__VA_ARGS__)
#define CLIENT_INFO(...) ::ProjectA::Logger::clientLogger.info(__VA_ARGS__)
#define CLIENT_WARN(...) ::ProjectA::Logger::clientLogger.warn(__VA_ARGS__)
#define CLIENT_ERROR(...) ::ProjectA::Logger::clientLogger.error(__VA_ARGS__)
#define CLIENT_FATAL(...) ::ProjectA::Logger::clientLogger.fatal(__VA_ARGS__)

#define ENGINE_ASSERT(condition, ...) { \
	if (!(condition)) { \
		ENGINE_ERROR(__VA_ARGS__); \
		BREAKPOINT(); \
	} \
}

#define CLIENT_ASSERT(condition, ...) { \
	if (!(condition)) { \
		CLIENT_ERROR(__VA_ARGS__); \
		BREAKPOINT(); \
	} \
}

#define ENGINE_STATIC_ASSERT static_assert

#include "Engine/Base/UDL.hpp"

#ifdef ENABLE_OPENGL_ADAPTER
	#include "Backend/OpenGL/base.hpp"
#endif

#ifdef ENABLE_VULKAN_ADAPTER
	#include "Backend/Vulkan/base.hpp"
#endif

#include "Engine/Base/sequence.hpp"
#include "Engine/Base/Profile.hpp"
#include "Engine/Base/String.hpp"