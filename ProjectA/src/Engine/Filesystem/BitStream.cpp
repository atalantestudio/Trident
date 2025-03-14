#include "pch.hpp"

#include "Engine/Filesystem/BitStream.hpp"

namespace ProjectA {
	BitStream::BitStream(const uint8* stream, uint64 size) :
		stream(stream),
		size(size),
		bitBuffer(*(acc_t*)(stream))
	{}

	void BitStream::jump(uint64 bitCount) {
		ENGINE_ASSERT(bitCount <= getRemainingSize(), "BitStream: Attempted to read [] bits, but only [] were left.", bitCount, getRemainingSize());

		uint8 jumpBitCount = 0;

		if (bitCount > static_cast<uint8>(7 - bitOffset)) {
			ENGINE_DEBUG("Bit buffer overflow; reading next bytes.");

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
}