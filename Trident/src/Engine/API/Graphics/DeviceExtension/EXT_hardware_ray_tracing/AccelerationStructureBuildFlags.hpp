// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class AccelerationStructureBuildFlags : uint8 {
		ALLOW_UPDATE = BIT(0),
		ALLOW_COMPACTION = BIT(1),
		PREFER_FAST_TRACE = BIT(2),
		PREFER_FAST_BUILD = BIT(3),
		MINIMIZE_MEMORY = BIT(4),
	};

	AS_FLAG(AccelerationStructureBuildFlags);
}