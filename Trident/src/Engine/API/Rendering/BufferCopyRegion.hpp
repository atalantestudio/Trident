#pragma once

namespace Trident {
	struct BufferCopyRegion {
		uint64 sourceOffset;
		uint64 destinationOffset;
		uint64 size;
	};
}