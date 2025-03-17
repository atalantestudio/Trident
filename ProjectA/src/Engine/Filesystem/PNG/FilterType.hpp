#pragma once

namespace ProjectA {
	enum class FilterType : uint8 {
		NONE = 0,
		SUB = 1,
		UP = 2,
		AVERAGE = 3,
		PAETH = 4,
	};
}