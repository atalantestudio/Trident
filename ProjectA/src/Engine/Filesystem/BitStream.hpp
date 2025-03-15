#pragma once

namespace ProjectA {
	class BitStream {
		public:
			typedef uint64 acc_t;

		public:
			// TODO: Should stream be acc_t*?
			BitStream(const uint8* stream, uint64 size);

			// Advances by the provided bit count without reading the values.
			// Usage:
			// - The bit count must be <= the remaining bits in the stream.
			void jump(uint64 bitCount);

			// Returns the next bits from the stream without updating the offsets.
			// Usage:
			// - The bit count must be <= the number of bits in T.
			// - The bit count must be <= MAX_PEEK_BIT_COUNT.
			// - The bit count must be <= the remaining bits in the stream.
			template<typename T>
			T peek(uint8 bitCount) const {
				ENGINE_ASSERT(bitCount <= sizeof(T) * 8, "BitStream: Cannot peek [] bits for a []-bit type.", static_cast<int32>(bitCount), sizeof(T) * 8);
				ENGINE_ASSERT(bitCount <= MAX_READ_OR_PEEK_BIT_COUNT, "BitStream: Cannot peek more than [] bits at once.", MAX_READ_OR_PEEK_BIT_COUNT);
				ENGINE_ASSERT(bitCount <= getRemainingSize(), "BitStream: Attempted to peek [] bits, but only [] were left.", static_cast<int32>(bitCount), getRemainingSize());

				T value = static_cast<T>(bitBuffer);
				value |= (*(acc_t*)(stream + sizeof(acc_t))) << (sizeof(acc_t) * 8 - bitOffset);

				return value & (static_cast<T>(1) << bitCount) - static_cast<T>(1);
			}

			// Returns the next bits from the stream.
			// Usage:
			// - The bit count must be <= the number of bits in T.
			// - The bit count must be <= MAX_READ_OR_PEEK_BIT_COUNT.
			// - The bit count must be <= the remaining bits in the stream.
			template<typename T>
			T read(uint8 bitCount) {
				ENGINE_ASSERT(bitCount <= sizeof(T) * 8, "BitStream: Cannot read [] bits for a []-bit type.", static_cast<int32>(bitCount), sizeof(T) * 8);
				ENGINE_ASSERT(bitCount <= MAX_READ_OR_PEEK_BIT_COUNT, "BitStream: Cannot read more than [] bits at once.", MAX_READ_OR_PEEK_BIT_COUNT);
				ENGINE_ASSERT(bitCount <= getRemainingSize(), "BitStream: Attempted to read [] bits, but only [] were left.", static_cast<int32>(bitCount), getRemainingSize());

				T value = static_cast<T>(bitBuffer);
				uint8 jumpBitCount = 0;

				if (bitCount > static_cast<uint8>((sizeof(acc_t) * 8 - 1) - bitOffset)) {
					jumpBitCount = bitOffset;

					const uint64 jumpByteCount = (bitOffset + bitCount) / (sizeof(acc_t) * 8);

					byteOffset += sizeof(acc_t) * jumpByteCount;

					refillBitBuffer();

					value |= bitBuffer << (sizeof(acc_t) * 8 - bitOffset);

					bitOffset = 0;
				}

				jumpBitCount = (jumpBitCount + bitCount) % (sizeof(acc_t) * 8);

				bitOffset += jumpBitCount;
				bitBuffer >>= jumpBitCount;

				return value & (static_cast<T>(1) << bitCount) - static_cast<T>(1);
			}

		private:
			uint64 getRemainingSize() const;

			void refillBitBuffer();

		private:
			static constexpr uint8 MAX_READ_OR_PEEK_BIT_COUNT = 64;

		private:
			const uint8* stream;

			// Bit accumulator.
			// Missed opportunity to name this "bitBucket".
			acc_t bitBuffer;

			// Size of the stream in bytes.
			uint64 size;

			// Offset of the currently read byte.
			uint64 byteOffset = 0;

			// Offset within the bit buffer.
			// E [0, 64[
			uint8 bitOffset: 6;

			bool swapEndianness = false;
	};
}