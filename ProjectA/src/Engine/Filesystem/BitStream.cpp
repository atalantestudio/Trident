#include "pch.hpp"

#include "Engine/Filesystem/BitStream.hpp"

namespace ProjectA {
	BitStream::BitStream(const uint8* stream, uint64 size) :
		stream(stream),
		size(size),
		bitOffset(0)
	{
		refillBitBuffer();
	}

	void BitStream::jump(uint64 bitCount) {
		ENGINE_ASSERT(bitCount <= getRemainingSize(), "BitStream: Attempted to jump [] bits, but only [] were left.", bitCount, getRemainingSize());

		uint8 jumpBitCount = 0;

		if (bitCount > static_cast<uint8>((sizeof(acc_t) * 8 - 1) - bitOffset)) {
			jumpBitCount = bitOffset;

			const uint64 jumpByteCount = (bitOffset + bitCount) / (sizeof(acc_t) * 8);

			bitOffset = 0;
			byteOffset += sizeof(acc_t) * jumpByteCount;

			refillBitBuffer();
		}

		jumpBitCount = (jumpBitCount + bitCount) % (sizeof(acc_t) * 8);

		bitOffset += jumpBitCount;
		bitBuffer >>= jumpBitCount;
	}

	uint64 BitStream::getRemainingSize() const {
		return (size - byteOffset) * 8 - bitOffset;
	}

	void BitStream::refillBitBuffer() {
		bitBuffer = *(acc_t*)(stream + byteOffset);

		if (swapEndianness) {
			bitBuffer = _byteswap_uint64(bitBuffer);
		}
	}
}