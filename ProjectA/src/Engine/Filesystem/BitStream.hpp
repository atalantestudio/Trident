#pragma once

namespace ProjectA {
	class BitStream {
		public:
			typedef uint8 acc_t;

		public:
			BitStream(const acc_t* stream, uint64 size);

			template<typename T>
			T read(uint32 bitCount) {
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

			// Returns the next bits without incrementing the offsets.
			// The bit count must be less than or equal to PEEK_LIMIT.
			acc_t peek(uint8 bitCount) const {
				/*ENGINE_ASSERT(bitCount <= PEEK_LIMIT, "BitStream: Cannot peek more than [] bits at once.", static_cast<uint16>(PEEK_LIMIT));
				// TODO: Remaining stream size assertion.

				const uint8 remainingBufferBits = PEEK_LIMIT - relativeBitOffset;

				if (relativeBitOffset == 0) {
					// Bit buffer is empty - can't take bits from it.
				}

				acc_t value = 0;

				const acc_t mask = (static_cast<acc_t>(1) << bitCount) - 1;

				if (relativeBitOffset > 0 && remainingBufferBits >= bitCount) {
					// Take what we need from the bit buffer and mask the rest.
					value = bitBuffer & mask;
				}

				// Read from the stream without advancing the pointer.
				const acc_t next8Bytes = *((acc_t*) (stream + ptr));

				value |= (next8Bytes);

				value &= mask;

				return value;*/
			}

			// Advances by the provided bit count without reading the values.
			void jump(uint64 bitCount);

		private:
			uint64 getRemainingSize() const {
				return (size - byteOffset) * 8 - bitOffset;
			}

		private:
			static constexpr uint8 MAX_READ_BITS = sizeof(acc_t) * 8;
			static constexpr uint8 MAX_PEEKED_BITS = sizeof(acc_t) * 8;

		private:
			const acc_t* stream;

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