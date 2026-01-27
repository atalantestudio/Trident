// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct BufferCopyRegion {
		uint64 sourceOffset;
		uint64 destinationOffset;
		uint64 size;
	};
}