#pragma once

#include "pch.hpp"

namespace ProjectA {
	enum class LogStructure : uint8 {
		TIME = 1 << 0,
		LOGGER_NAME = 1 << 1,
		LOG_LEVEL_NAME = 1 << 2,
	};

	AS_FLAG(LogStructure);
}