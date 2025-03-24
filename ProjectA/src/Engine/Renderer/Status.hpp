#pragma once

namespace ProjectA {
	enum class Status : uint8 {
		SUCCESS = 0,
		SUBOPTIMAL = 1,
		ERROR_OUT_OF_DATE = 2,
		ERROR_UNHANDLED = 3,
	};
}