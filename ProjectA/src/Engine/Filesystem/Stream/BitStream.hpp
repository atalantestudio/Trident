#pragma once

#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace ProjectA {
	class BitStream {
		private:
			typedef uint64 StreamSize;

		public:
			BitStream(const uint8* stream, uint64 byteSize);

			uint64 getBitOffset() const;

			const uint8* getBytePosition() const;

			uint64 getRemainingBitSize() const;

			// Advances the offsets by the provided bit count.
			// Usage:
			// - The bit count must be <= the remaining bits in the stream.
			void jump(uint64 bitCount);

			// Returns the next bits from the stream without updating the offsets.
			// Usage:
			// - The bit count must be <= the number of bits in T.
			// - The bit count must be <= MAX_READ_OR_PEEK_BIT_COUNT.
			// - The bit count must be <= the remaining bits in the stream.
			template<typename T>
			T peek(uint8 bitCount, bool swapEndianness = false) const {
				ENGINE_ASSERT(bitCount <= sizeof(T) * 8, "Cannot peek [] bits for a []-bit type.", static_cast<sint32>(bitCount), sizeof(T) * 8);
				ENGINE_ASSERT(bitCount <= MAX_READ_OR_PEEK_BIT_COUNT, "Cannot peek more than [] bits at once.", MAX_READ_OR_PEEK_BIT_COUNT);

				const uint64 remainingBitSize = getRemainingBitSize();

				ENGINE_ASSERT(bitCount <= remainingBitSize, "Attempted to peek [] bits, but only [] were remaining.", static_cast<sint32>(bitCount), remainingBitSize);

				T value = static_cast<T>(bitBuffer);

				if (swapEndianness) {
					value = ByteStream::byteswap<T>(value);
				}

				if (bitOffset > 0) {
					const StreamSize offset = sizeof(bitBuffer) * 8 - bitOffset;

					value |= (*((StreamSize*) (stream + byteOffset + sizeof(bitBuffer)))) << offset;
				}

				return value & ((1ULL << bitCount) - 1);
			}

			// Returns the next bits from the stream.
			// Usage:
			// - The bit count must be <= the number of bits in T.
			// - The bit count must be <= MAX_READ_OR_PEEK_BIT_COUNT.
			// - The bit count must be <= the remaining bits in the stream.
			template<typename T>
			T read(uint8 bitCount, bool swapEndianness = false) {
				const T value = peek<T>(bitCount, swapEndianness);

				jump(bitCount);

				return value;
			}

			// Decompresses data using the DEFLATE compression algorithm.
			// References:
			// - DEFLATE Compressed Data Format Specification (https://datatracker.ietf.org/doc/html/rfc1950)
			// - ZLIB Compressed Data Format Specification (https://datatracker.ietf.org/doc/html/rfc1951)
			sequence<uint8> inflate(uint64 uncompressedSize);

		private:
			static constexpr uint8 MAX_READ_OR_PEEK_BIT_COUNT = sizeof(StreamSize) * 8;

		private:
			// TODO: Should stream be StreamSize* instead?
			const uint8* stream;

			// Bit accumulator.
			// Missed opportunity to name this "bitBucket".
			StreamSize bitBuffer;

			// Size of the stream in bytes.
			uint64 byteSize;

			// Offset of the currently read byte.
			uint64 byteOffset = 0;

			// Offset within the bit buffer.
			// E [0, 64[
			uint8 bitOffset: 6;
	};
}