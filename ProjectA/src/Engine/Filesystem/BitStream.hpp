#pragma once

namespace ProjectA {
	class BitStream {
		public:
			typedef uint8 acc_t;

		public:
			BitStream(const uint8* stream, uint64 size);

			// Returns the next bits from the stream.
			// The bit count must be less than or equal to MAX_READ_OR_PEEK_BIT_COUNT.
			template<typename T>
			T read(uint8 bitCount) {
				ENGINE_ASSERT(bitCount <= sizeof(T) * 8, "BitStream: Cannot read [] bits for a []-bit type.", static_cast<int32>(bitCount), sizeof(T) * 8);
				ENGINE_ASSERT(bitCount <= MAX_READ_OR_PEEK_BIT_COUNT, "BitStream: Cannot read more than [] bits at once.", MAX_READ_OR_PEEK_BIT_COUNT);
				ENGINE_ASSERT(bitCount <= getRemainingSize(), "BitStream: Attempted to read [] bits, but only [] were left.", static_cast<int32>(bitCount), getRemainingSize());

				return 0;

				/*ENGINE_ASSERT(bitCount <= sizeof(bitBuffer) * 8, "BitStream: Cannot read more than [] bits at a time.", sizeof(bitBuffer) * 8);

				// Read until the bit buffer has enough bits.
				while (bitBufferCount < bitCount) {
					//const uint32 byte = stream.get();
					const uint8 byte = *(stream + ptr);
					ptr += sizeof(bitBuffer);

					// OR the byte into the bit buffer at the next position.
					bitBuffer |= byte << bitBufferCount;
					bitBufferCount += sizeof(bitBuffer) * 8;
				}

				//const uint8 value = bitBuffer & ((1 << bitCount) - 1);

				// Right shift the bit buffer by the number of bits read.
				bitBuffer >>= bitCount;
				bitBufferCount -= bitCount;

				return value;*/
			}

			// Returns the next bits from the stream without updating the offsets.
			// The bit count must be less than or equal to MAX_PEEK_BIT_COUNT.
			template<typename T>
			T peek(uint8 bitCount) const {
				ENGINE_ASSERT(bitCount <= sizeof(T) * 8, "BitStream: Cannot peek [] bits for a []-bit type.", static_cast<int32>(bitCount), sizeof(T) * 8);
				ENGINE_ASSERT(bitCount <= MAX_READ_OR_PEEK_BIT_COUNT, "BitStream: Cannot peek more than [] bits at once.", MAX_READ_OR_PEEK_BIT_COUNT);
				ENGINE_ASSERT(bitCount <= getRemainingSize(), "BitStream: Attempted to peek [] bits, but only [] were left.", static_cast<int32>(bitCount), getRemainingSize());

				// Pick what's left of the bit buffer.
				T value = bitBuffer;

				// Right shift the next byte(s) of the stream.
				value |= (*(acc_t*)(stream + sizeof(acc_t))) << (8 - bitOffset);

				// Mask the rest.
				value &= (1 << bitCount) - 1;

				return value;
			}

			// Advances by the provided bit count without reading the values.
			void jump(uint64 bitCount);

		private:
			uint64 getRemainingSize() const {
				return (size - byteOffset) * 8 - bitOffset;
			}

		private:
			static constexpr uint8 MAX_READ_OR_PEEK_BIT_COUNT = 64;

		private:
			const uint8* stream;

			// Size of the stream in bytes.
			uint64 size;

			// Offset of the currently read byte.
			uint64 byteOffset = 0;

			// Offset within the bit buffer.
			uint8 bitOffset = 0;

			// Bit accumulator.
			// Missed opportunity to name this "bitBucket".
			acc_t bitBuffer;
	};
}