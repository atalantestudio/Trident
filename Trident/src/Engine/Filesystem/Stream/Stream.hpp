// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	template<typename T>
	inline constexpr T byteswap(T value) {
		switch (sizeof(T)) {
			case 1: {
				return value;
			}
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
			default: {
				ENGINE_ASSERT(false, "Unable to byteswap value.");

				return value;
			}
		}
	}

	template<>
	inline sint16 byteswap(sint16 value) {
		return _byteswap_ushort(value);
	}

	template<>
	inline uint16 byteswap(uint16 value) {
		return _byteswap_ushort(value);
	}

	template<>
	inline sint32 byteswap(sint32 value) {
		return _byteswap_ulong(value);
	}

	template<>
	inline uint32 byteswap(uint32 value) {
		return _byteswap_ulong(value);
	}

	template<>
	inline sint64 byteswap(sint64 value) {
		return _byteswap_uint64(value);
	}

	template<>
	inline uint64 byteswap(uint64 value) {
		return _byteswap_uint64(value);
	}

	// A generic data stream.
	class Stream {
		protected:
			explicit Stream(uint64 size, Endianness endianness) :
				size(size),
				endianness(endianness)
			{}

		public:
			uint64 getByteOffset() const {
				return byteOffset;
			}

			// Returns the remaining byte size.
			uint64 getRemainingSize() const {
				return size - byteOffset;
			}

			Endianness getEndianness() const {
				return endianness;
			}

			// Advances by the provided byte count.
			// `byteCount` must be less than or equal to the remaining bytes in the offsetted stream.
			void jump(uint64 byteCount) {
				const uint64 remainingByteSize = getRemainingSize();

				ENGINE_ASSERT(byteCount <= remainingByteSize, "Attempted to jump [] byte(s), but only [] were remaining.", byteCount, remainingByteSize);

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