#pragma once

#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace ProjectA {
	template<StreamOperation StreamOperation>
	class BitStream;

	template<>
	class BitStream<StreamOperation::READ> {
		private:
			typedef uint64 BitStreamSize;

		public:
			explicit BitStream(const uint8* data, uint64 byteSize) :
				data(data),
				bitBuffer(*pointer_cast<const BitStreamSize*>(&data[0])),
				byteSize(byteSize),
				bitOffset(0)
			{}

			uint64 getBitOffset() const {
				return byteOffset * 8 + bitOffset;
			}

			const uint8* getBytePosition() const {
				return data + byteOffset + bitOffset / 8;
			}

			uint64 getRemainingBitSize() const {
				return (byteSize - byteOffset) * 8 - bitOffset;
			}

			// Advances the offsets by the provided bit count.
			// The bit count must be <= the remaining bits in the stream.
			void jump(uint64 bitCount) {
				const uint64 remainingBitSize = getRemainingBitSize();

				ENGINE_ASSERT(bitCount <= remainingBitSize, "Attempted to jump [] bits, but only [] were remaining.", bitCount, remainingBitSize);

				uint8 jumpBitCount = 0;

				if (bitCount > static_cast<uint8>((sizeof(BitStreamSize) * 8 - 1) - bitOffset)) {
					jumpBitCount = bitOffset;

					const uint64 jumpByteCount = (bitOffset + bitCount) / (sizeof(bitBuffer) * 8);

					bitOffset = 0;
					byteOffset += sizeof(bitBuffer) * jumpByteCount;
					bitBuffer = *(BitStreamSize*)(data + byteOffset);
				}

				jumpBitCount = (jumpBitCount + bitCount) % (sizeof(bitBuffer) * 8);

				bitOffset += jumpBitCount;
				bitBuffer >>= jumpBitCount;
			}

			// Returns the next bits from the stream without updating the offsets.
			// The bit count must be <= the number of bits in T.
			// The bit count must be <= MAX_READ_OR_PEEK_BIT_COUNT.
			// The bit count must be <= the remaining bits in the stream.
			template<typename T>
			T peek(uint8 bitCount, bool swapEndianness = false) const {
				ENGINE_ASSERT(bitCount <= sizeof(T) * 8, "Cannot peek [] bits for a []-bit type.", static_cast<sint32>(bitCount), sizeof(T) * 8);
				ENGINE_ASSERT(bitCount <= MAX_READ_OR_PEEK_BIT_COUNT, "Cannot peek more than [] bits at once.", MAX_READ_OR_PEEK_BIT_COUNT);

				const uint64 remainingBitSize = getRemainingBitSize();

				ENGINE_ASSERT(bitCount <= remainingBitSize, "Attempted to peek [] bits, but only [] were remaining.", static_cast<sint32>(bitCount), remainingBitSize);

				T value = *pointer_cast<const T*>(&bitBuffer);

				if (swapEndianness) {
					value = Stream::byteswap(value);
				}

				BitStreamSize valueStreamSize = *pointer_cast<const BitStreamSize*>(&value);

				if (bitOffset > 0) {
					const BitStreamSize offset = sizeof(bitBuffer) * 8 - bitOffset;

					valueStreamSize |= (*((BitStreamSize*) (data + byteOffset + sizeof(bitBuffer)))) << offset;
				}

				valueStreamSize &= ((1ULL << bitCount) - 1);

				return *pointer_cast<const T*>(&valueStreamSize);
			}

			// Returns the next bits from the stream.
			// The bit count must be <= the number of bits in T.
			// The bit count must be <= MAX_READ_OR_PEEK_BIT_COUNT.
			// The bit count must be <= the remaining bits in the stream.
			template<typename T>
			T read(uint8 bitCount, bool swapEndianness = false) {
				const T value = peek<T>(bitCount, swapEndianness);

				jump(bitCount);

				return value;
			}

			// TODO: Implement seek<T>(uint8 bitCount).
			// TODO: Implement write<T>(uint8 bitCount).

			// Uncompresses data using the DEFLATE decompression algorithm.
			sequence<uint8> inflate(uint64 uncompressedSize);

		private:
			void parseUncompressedZlibBlock(uint8* data);

			void parseCompressedZlibBlock(uint8* data, uint64& dataOffset, uint8 BTYPE);

		private:
			static constexpr uint8 MAX_READ_OR_PEEK_BIT_COUNT = 8 * sizeof(BitStreamSize);

		private:
			// TODO: Should stream be BitStreamSize* instead?
			const uint8* data;

			// Bit accumulator (missed opportunity to name this "bitBucket").
			BitStreamSize bitBuffer = 0;

			// Size of the stream in bytes.
			uint64 byteSize;

			// Offset of byte that is currently being read.
			uint64 byteOffset = 0;

			// Offset within the bit buffer.
			uint8 bitOffset: 6;
	};
}