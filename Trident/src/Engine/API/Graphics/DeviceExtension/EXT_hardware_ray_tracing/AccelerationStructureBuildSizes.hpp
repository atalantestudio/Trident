// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct AccelerationStructureBuildSizes {
		/// The size in bytes required for building or updating an acceleration structure.
		uint64 accelerationStructureSize;

		/// The size in bytes required for building a scratch buffer.
		uint64 scratchBuildSize;

		/// The size in bytes required for updating a scratch buffer.
		uint64 scratchUpdateSize;
	};
}