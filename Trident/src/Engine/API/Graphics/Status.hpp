// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
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