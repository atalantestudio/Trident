#pragma once

// NOTE: This file and every other file in the TSC/Base directory are used by Emscripten for the VS Code extension.

#include <string>
#include <vector>

#include <emscripten/bind.h>

#include "Context.hpp"
#include "Logger.hpp"
#include "Macros.hpp"

#include "Bit.hpp"
#include "Enum.hpp" // Depends on Types.hpp

#include "Types.hpp"
#include "Utilities.hpp" // Depends on Types.hpp