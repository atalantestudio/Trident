#pragma once

#include "pch.hpp"

namespace ProjectA {
	enum class LogStructure : uint8 {
		NONE = 0,
		TIME = BIT(0),
		LOGGER_NAME = BIT(1),
		LOG_LEVEL_NAME = BIT(2),
	};

	AS_FLAG(LogStructure);
}