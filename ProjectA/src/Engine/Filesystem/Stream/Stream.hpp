#pragma once

namespace ProjectA {
	class Stream {
		public:
			template<typename T>
			static constexpr T byteswap(T value) {
				switch (sizeof(T)) {
					case 1:
						return value;
					case 2: {
						const uint16 valueInteger = *pointer_cast<const uint16*>(&value);

						return *pointer_cast<const T*>(&valueInteger);
					}
					case 4: {
						const uint32 valueInteger = *pointer_cast<const uint32*>(&value);

						return *pointer_cast<const T*>(&valueInteger);
					}
					case 8: {
						const uint64 valueInteger = *pointer_cast<const uint64*>(&value);

						return *pointer_cast<const T*>(&valueInteger);
					}
					default:
						ENGINE_ASSERT(false, "Unable to byteswap value.");

						return value;
				}
			}

			template<>
			static sint16 byteswap(sint16 value) {
				return _byteswap_ushort(value);
			}

			template<>
			static uint16 byteswap(uint16 value) {
				return _byteswap_ushort(value);
			}

			template<>
			static sint32 byteswap(sint32 value) {
				return _byteswap_ulong(value);
			}

			template<>
			static uint32 byteswap(uint32 value) {
				return _byteswap_ulong(value);
			}

			template<>
			static sint64 byteswap(sint64 value) {
				return _byteswap_uint64(value);
			}

			template<>
			static uint64 byteswap(uint64 value) {
				return _byteswap_uint64(value);
			}

		protected:
			explicit Stream(uint64 size, Endianness endianness) :
				size(size),
				endianness(endianness)
			{}

		public:
			uint64 getByteOffset() const {
				return byteOffset;
			}

			uint64 getRemainingByteSize() const {
				return size - byteOffset;
			}

			Endianness getEndianness() const {
				return endianness;
			}

			// Advances by the provided byte count.
			// `byteCount` must be less than or equal to the remaining bytes in the offsetted stream.
			void jump(uint64 byteCount) {
				const uint64 remainingByteSize = getRemainingByteSize();

				ENGINE_ASSERT(byteCount <= remainingByteSize, "Attempted to jump [] bytes, but only [] were remaining.", byteCount, remainingByteSize);

				byteOffset += byteCount;
			}

			// Advances the offset relatively to the start of the stream.
			void seek(uint64 byteOffset) {
				ENGINE_ASSERT(byteOffset <= size, "Cannot seek more bytes than there is in the stream.", byteOffset, size);

				this->byteOffset = byteOffset;
			}

		public:
			// Stream size in bytes.
			const uint64 size;

		protected:
			// Offset of the currently read byte.
			uint64 byteOffset = 0;

			Endianness endianness;
	};
}