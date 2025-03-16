#include "pch.hpp"

#include "Engine/Filesystem/BitStream.hpp"

namespace ProjectA {
	BitStream::BitStream(const uint8* stream, uint64 byteSize) :
		stream(stream),
		byteSize(byteSize),
		bitOffset(0)
	{
		bitBuffer = *(acc_t*)(stream + byteOffset);
	}

	const uint8* BitStream::getBytePosition() const {
		return stream + byteOffset;
	}

	uint64 BitStream::getRemainingBitSize() const {
		return (byteSize - byteOffset) * 8 - bitOffset;
	}

	void BitStream::jump(uint64 bitCount) {
		ENGINE_ASSERT(bitCount <= getRemainingBitSize(), "BitStream: Attempted to jump [] bits, but only [] were left.", bitCount, getRemainingBitSize());

		uint8 jumpBitCount = 0;

		if (bitCount > static_cast<uint8>((sizeof(acc_t) * 8 - 1) - bitOffset)) {
			jumpBitCount = bitOffset;

			const uint64 jumpByteCount = (bitOffset + bitCount) / (sizeof(acc_t) * 8);

			bitOffset = 0;
			byteOffset += sizeof(acc_t) * jumpByteCount;
			bitBuffer = *(acc_t*)(stream + byteOffset);
		}

		jumpBitCount = (jumpBitCount + bitCount) % (sizeof(acc_t) * 8);

		bitOffset += jumpBitCount;
		bitBuffer >>= jumpBitCount;
	}

	void BitStream::close() {
		byteOffset = byteSize;
		bitOffset = 0;
	}
}