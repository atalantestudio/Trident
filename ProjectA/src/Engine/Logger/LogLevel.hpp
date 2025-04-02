#pragma once

namespace ProjectA {
	enum class LogLevel : uint8 {
		NONE = 7,
		TRACE = 1,
		DEBUG = 2,
		INFO = 3,
		WARN = 4,
		ERROR = 5,
		FATAL = 6,
		ALL = 0,
	};
}