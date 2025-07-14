#pragma once

namespace ProjectA {
	enum class TimeOfImpactState : uint8 {
		UNDETERMINED = 0,
		SEPARATED = 1,
		HIT = 2,
		OVERLAPPED = 3,
		UNRESOLVED_ROOT = 4,
	};

	AS_PRINTABLE(TimeOfImpactState);
}