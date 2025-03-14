#include "pch.hpp"

#include "Engine/Filesystem/BitStream.hpp"

namespace ProjectA {
	BitStream::BitStream(const acc_t* stream, uint64 size) :
		stream(stream),
		size(size),
		bitBuffer(*(acc_t*)(stream))
	{}

	void BitStream::jump(uint64 bitCount) {
		ENGINE_ASSERT(bitCount <= getRemainingSize(), "BitStream: Attempted to read [] bits, but only [] were left.", bitCount, getRemainingSize());

		// Decompose the jump into bit and byte parts.
		uint64 jumpBitCount = bitCount % 8;
		uint64 jumpByteCount = 0;

		if (bitCount > static_cast<uint8>(7 - bitOffset)) {
			ENGINE_DEBUG("Bit buffer overflow; reading next bytes.");

			jumpBitCount = (bitOffset + bitCount) % 8;
			jumpByteCount = (bitOffset + bitCount) / 8;

			byteOffset += sizeof(acc_t) * jumpByteCount;
			bitBuffer = *(acc_t*)(stream + byteOffset);

			// Reset the bit offset.
			bitOffset = 0;
		}

		// Increment the bit offset.
		bitOffset += static_cast<uint8>(jumpBitCount);

		// Right shift the bit buffer.
		bitBuffer >>= jumpBitCount;
	}
}