#pragma once

namespace ProjectA {
	enum class LogLevel : uint8 {
		ALL,
		TRACE,
		DEBUG,
		INFO,
		WARN,
		ERROR,
		FATAL,
		NONE,
	};
}