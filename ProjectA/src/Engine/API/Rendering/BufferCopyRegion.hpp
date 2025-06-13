#pragma once

namespace ProjectA {
	struct BufferCopyRegion {
		uint64 sourceOffset;
		uint64 destinationOffset;
		uint64 size;
	};
}