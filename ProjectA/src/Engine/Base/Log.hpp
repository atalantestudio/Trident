#pragma once

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