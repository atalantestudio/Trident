#pragma once

namespace ProjectA {
	enum class AcquireNextImageStatus : uint8 {
		SUCCESS = 0,
		SUBOPTIMAL = 1,
		ERROR_OUT_OF_DATE = 2,
		ERROR_UNHANDLED = 3,
	};

	enum class PresentStatus : uint8 {
		SUCCESS = 0,
		SUBOPTIMAL = 1,
		ERROR_OUT_OF_DATE = 2,
		ERROR_UNHANDLED = 3,
	};
}