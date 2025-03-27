#include "pch.hpp"

#include "Engine/Filesystem/Stream/BitStream.hpp"

namespace ProjectA {
	BitStream::BitStream(const uint8* stream, uint64 byteSize) :
		stream(stream),
		byteSize(byteSize),
		bitOffset(0)
	{
		bitBuffer = *(StreamSize*)(stream + byteOffset);
	}

	uint64 BitStream::getBitOffset() const {
		return byteOffset * 8 + bitOffset;
	}

	const uint8* BitStream::getBytePosition() const {
		return stream + byteOffset;
	}

	uint64 BitStream::getRemainingBitSize() const {
		return (byteSize - byteOffset) * 8 - bitOffset;
	}

	void BitStream::jump(uint64 bitCount) {
		const uint64 remainingBitSize = getRemainingBitSize();

		ENGINE_ASSERT(bitCount <= remainingBitSize, "Attempted to jump [] bits, but only [] were remaining.", bitCount, remainingBitSize);

		uint8 jumpBitCount = 0;

		if (bitCount > static_cast<uint8>((sizeof(StreamSize) * 8 - 1) - bitOffset)) {
			jumpBitCount = bitOffset;

			const uint64 jumpByteCount = (bitOffset + bitCount) / (sizeof(bitBuffer) * 8);

			bitOffset = 0;
			byteOffset += sizeof(bitBuffer) * jumpByteCount;
			bitBuffer = *(StreamSize*)(stream + byteOffset);
		}

		jumpBitCount = (jumpBitCount + bitCount) % (sizeof(bitBuffer) * 8);

		bitOffset += jumpBitCount;
		bitBuffer >>= jumpBitCount;
	}
}