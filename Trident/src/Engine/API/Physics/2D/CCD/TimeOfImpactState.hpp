// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class TimeOfImpactState : uint8 {
		UNDETERMINED = 0,
		SEPARATED = 1,
		HIT = 2,
		OVERLAPPED = 3,
		UNRESOLVED_ROOT = 4,
	};

	AS_PRINTABLE(TimeOfImpactState);
}