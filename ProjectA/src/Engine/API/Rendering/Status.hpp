#pragma once

namespace ProjectA {
	enum class AcquireNextImageStatus : uint8 {
		SUCCESS,
		SUBOPTIMAL,
		ERROR_OUT_OF_DATE,
		ERROR_UNHANDLED,
	};

	enum class PresentStatus : uint8 {
		SUCCESS,
		SUBOPTIMAL,
		ERROR_OUT_OF_DATE,
		ERROR_UNHANDLED,
	};
}