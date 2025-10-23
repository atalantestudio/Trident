#pragma once

// NOTE: This file and every other file in the TSC/Base directory are used by Emscripten for the VS Code extension.

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <emscripten/bind.h>

#include "Context.hpp"
#include "Macros.hpp"

#include "Bit.hpp"
#include "Cast.hpp"
#include "Enum.hpp" // Depends on Types.hpp

#include "Types.hpp"
#include "Utilities.hpp" // Depends on Types.hpp

#include "../../../../ProjectA/src/Engine/Logger/Logger.hpp"

// TODO: Create dedicated module logger instance.
#define MODULE_FORMAT(...) ::ProjectA::Logger::engineLogger.format(__VA_ARGS__)
#define MODULE_TRACE(argumentOrPattern, ...) ::ProjectA::Logger::engineLogger.trace(argumentOrPattern "\n    at []\n    ([]:[])", ##__VA_ARGS__, __FUNCSIG__, __FILE__, __LINE__)
#define MODULE_TRACE_PREFORMATTED(...) ::ProjectA::Logger::engineLogger.trace(__VA_ARGS__)
#define MODULE_DEBUG(...) ::ProjectA::Logger::engineLogger.debug(__VA_ARGS__)
#define MODULE_INFO(...) ::ProjectA::Logger::engineLogger.info(__VA_ARGS__)
#define MODULE_WARN(...) ::ProjectA::Logger::engineLogger.warn(__VA_ARGS__)
#define MODULE_ERROR(...) ::ProjectA::Logger::engineLogger.error(__VA_ARGS__)