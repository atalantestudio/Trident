#pragma once

namespace ProjectA {
	class BitStream {
		public:
			typedef uint64 acc_t;

		public:
			BitStream(const uint8* stream, uint64 byteSize);

			uint64 getBitOffset() const;

			const uint8* getBytePosition() const;

			uint64 getRemainingBitSize() const;

			uint64 getRemainingByteSize() const;

			// Advances the offsets by the provided bit count.
			// Usage:
			// - The bit count must be <= the remaining bits in the stream.
			void jump(uint64 bitCount);

			// TODO: Always return uint64?
			// Returns the next bits from the stream without updating the offsets.
			// Usage:
			// - The bit count must be <= the number of bits in T.
			// - The bit count must be <= MAX_PEEK_BIT_COUNT.
			// - The bit count must be <= the remaining bits in the stream.
			template<typename T>
			T peek(uint8 bitCount, bool swapEndianness = false) const {
				ENGINE_ASSERT(bitCount <= sizeof(T) * 8, "BitStream: Cannot peek [] bits for a []-bit type.", static_cast<sint32>(bitCount), sizeof(T) * 8);
				ENGINE_ASSERT(bitCount <= MAX_READ_OR_PEEK_BIT_COUNT, "BitStream: Cannot peek more than [] bits at once.", MAX_READ_OR_PEEK_BIT_COUNT);
				ENGINE_ASSERT(bitCount <= getRemainingBitSize(), "BitStream: Attempted to peek [] bits, but only [] were left.", static_cast<sint32>(bitCount), getRemainingBitSize());

				T value = static_cast<T>(bitBuffer);

				if (swapEndianness) {
					switch (sizeof(value)) {
						case 2:
							value = static_cast<T>(_byteswap_ushort(value));

							break;
						case 4:
							value = static_cast<T>(_byteswap_ulong(value));

							break;
						case 8:
							value = static_cast<T>(_byteswap_uint64(value));

							break;
						default:
							ENGINE_ASSERT(false, "Could not byteswap []-byte value.", sizeof(value));
					}
				}

				value |= (*(acc_t*)(stream + sizeof(acc_t))) << (sizeof(acc_t) * 8 - bitOffset);

				return value & ((1ULL << bitCount) - 1);
			}

			// TODO: Always return uint64?
			// Returns the next bits from the stream (template version).
			// Usage:
			// - The bit count must be <= the number of bits in T.
			// - The bit count must be <= MAX_READ_OR_PEEK_BIT_COUNT.
			// - The bit count must be <= the remaining bits in the stream.
			template<typename T>
			T read(uint8 bitCount, bool swapEndianness = false) {
				ENGINE_ASSERT(bitCount <= sizeof(T) * 8, "BitStream: Cannot read [] bits for a []-bit type.", static_cast<sint32>(bitCount), sizeof(T) * 8);
				ENGINE_ASSERT(bitCount <= MAX_READ_OR_PEEK_BIT_COUNT, "BitStream: Cannot read more than [] bits at once.", MAX_READ_OR_PEEK_BIT_COUNT);
				ENGINE_ASSERT(bitCount <= getRemainingBitSize(), "BitStream: Attempted to read [] bits, but only [] were left.", static_cast<sint32>(bitCount), getRemainingBitSize());

				T value = static_cast<T>(bitBuffer);

				if (swapEndianness) {
					switch (sizeof(value)) {
						case 2:
							value = static_cast<T>(_byteswap_ushort(value));

							break;
						case 4:
							value = static_cast<T>(_byteswap_ulong(value));

							break;
						case 8:
							value = static_cast<T>(_byteswap_uint64(value));

							break;
						default:
							ENGINE_ASSERT(false, "Could not byteswap []-byte value.", sizeof(value));
					}
				}

				uint8 jumpBitCount = 0;

				if (bitCount > static_cast<uint8>((sizeof(acc_t) * 8 - 1) - bitOffset)) {
					jumpBitCount = bitOffset;

					const uint64 jumpByteCount = (bitOffset + bitCount) / (sizeof(acc_t) * 8);

					byteOffset += sizeof(acc_t) * jumpByteCount;
					bitBuffer = *(acc_t*)(stream + byteOffset);

					// TODO: If swapEndianness = true, a byteswap should happen here
					// because the bit buffer has been refilled.
					value |= bitBuffer << (sizeof(acc_t) * 8 - bitOffset);

					bitOffset = 0;
				}

				jumpBitCount = (jumpBitCount + bitCount) % (sizeof(acc_t) * 8);

				bitOffset += jumpBitCount;
				bitBuffer >>= jumpBitCount;

				return value & ((1ULL << bitCount) - 1);
			}

			// TODO: Avoid potential data loss warning.
			// Returns the next bits from the stream.
			// Usage:
			// - The bit count must be <= 64.
			// - The bit count must be <= MAX_READ_OR_PEEK_BIT_COUNT.
			// - The bit count must be <= the remaining bits in the stream.
			/*uint64 read(uint8 bitCount, bool swapEndianness = false) {
				ENGINE_ASSERT(bitCount <= sizeof(uint64) * 8, "BitStream: Cannot read [] bits for a []-bit type.", static_cast<sint32>(bitCount), sizeof(uint64) * 8);
				ENGINE_ASSERT(bitCount <= MAX_READ_OR_PEEK_BIT_COUNT, "BitStream: Cannot read more than [] bits at once.", MAX_READ_OR_PEEK_BIT_COUNT);
				ENGINE_ASSERT(bitCount <= getRemainingBitSize(), "BitStream: Attempted to read [] bits, but only [] were left.", static_cast<sint32>(bitCount), getRemainingBitSize());

				uint64 value = bitBuffer;

				if (swapEndianness) {
					value = _byteswap_uint64(value);
				}

				uint8 jumpBitCount = 0;

				if (bitCount > static_cast<uint8>((sizeof(acc_t) * 8 - 1) - bitOffset)) {
					jumpBitCount = bitOffset;

					const uint64 jumpByteCount = (bitOffset + bitCount) / (sizeof(acc_t) * 8);

					byteOffset += sizeof(acc_t) * jumpByteCount;
					bitBuffer = *(acc_t*)(stream + byteOffset);

					// TODO: If swapEndianness = true, a byteswap should happen here
					// because the bit buffer has been refilled.
					value |= bitBuffer << (sizeof(acc_t) * 8 - bitOffset);

					bitOffset = 0;
				}

				jumpBitCount = (jumpBitCount + bitCount) % (sizeof(acc_t) * 8);

				bitOffset += jumpBitCount;
				bitBuffer >>= jumpBitCount;

				return value & ((1ULL << bitCount) - 1);
			}*/

			// Advance the offsets to the end of the stream.
			void close();

		private:
			static constexpr uint8 MAX_READ_OR_PEEK_BIT_COUNT = 64;

		private:
			// TODO: Should stream be acc_t* instead?
			const uint8* stream;

			// Bit accumulator.
			// Missed opportunity to name this "bitBucket".
			acc_t bitBuffer;

			// Size of the stream in bytes.
			uint64 byteSize;

			// Offset of the currently read byte.
			uint64 byteOffset = 0;

			// Offset within the bit buffer.
			// E [0, 64[
			uint8 bitOffset: 6;
	};
}