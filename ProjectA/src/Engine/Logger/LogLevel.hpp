#pragma once

namespace ProjectA {
	enum class LogLevel : uint8 {
		ALL = 0,
		TRACE = 1,
		DEBUG = 2,
		INFO = 3,
		WARN = 4,
		ERROR = 5,
		FATAL = 6,
		NONE = 7,
	};
}