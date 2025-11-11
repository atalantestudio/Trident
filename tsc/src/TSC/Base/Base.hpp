#pragma once

// NOTE: This file and every other file in the TSC/Base directory are used by Emscripten for the VS Code extension.

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#ifdef __EMSCRIPTEN__
	#include <emscripten/bind.h>
#endif

#include "Macros.hpp"

#include "Bit.hpp"
#include "Enum.hpp" // Depends on Types.hpp

#include "Utilities.hpp" // Depends on Types.hpp

// TODO: Create dedicated module logger instance.
#define MODULE_FORMAT(...) ::ProjectA::Logger::format(__VA_ARGS__)

namespace ProjectA {
	static uint32 max(uint32 a, uint32 b) {
		if (a > b) {
			return a;
		}

		return b;
	}
}