#pragma once

namespace ProjectA {
	class ByteStream {
		public:
			template<typename T>
			static constexpr T byteswap(T value) {
				switch (sizeof(T)) {
					case 2: {
						const uint16 valueInteger = *pointer_cast_const<const uint16*>(&value);

						return *pointer_cast_const<const T*>(&valueInteger);
					}
					case 4: {
						const uint32 valueInteger = *pointer_cast_const<const uint32*>(&value);

						return *pointer_cast_const<const T*>(&valueInteger);
					}
					case 8: {
						const uint64 valueInteger = *pointer_cast_const<const uint64*>(&value);

						return *pointer_cast_const<const T*>(&valueInteger);
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

		public:
			ByteStream(const uint8* stream, uint64 size);

			uint64 getByteOffset() const;

			const uint8* getBytePosition() const;

			uint64 getRemainingByteSize() const;

			// Repositions the offset from the start of the stream.
			// `offset`: the stream offset in bytes.
			void seek(uint64 offset);

			// Advances by the provided byte count.
			// Usage:
			// `count`: The number of bytes to jump over. It must be less than or equal to the remaining bytes in the offsetted stream.
			void jump(uint64 count);

			// Returns the next bytes from the stream without updating the offsets.
			// Usage:
			// - The byte size of T must be <= the remaining bytes in the stream.
			template<typename T>
			T peek(bool swapEndianness = false) const {
				const uint64 remainingByteSize = getRemainingByteSize();

				ENGINE_ASSERT(sizeof(T) <= remainingByteSize, "Attempted to peek [] bytes, but only [] were remaining.", sizeof(T), remainingByteSize);

				T value = *pointer_cast_const<const T*>(stream + byteOffset);

				if (swapEndianness) {
					value = byteswap<T>(value);
				}

				return value;
			}

			// TODO: Allow entire structs to be read using ByteStream, e.g.
			// const MyStruct myStruct = stream.read<MyStruct>();
			// 
			// Returns the next bytes from the stream.
			// Usage:
			// - The byte size of T must be <= the remaining bytes in the stream.
			template<typename T>
			T read(bool swapEndianness = false) {
				const T value = peek<T>(swapEndianness);

				jump(sizeof(T));

				return value;
			}

		private:
			const uint8* stream;

			// Size of the stream in bytes.
			uint64 size;

			// Offset of the currently read byte.
			uint64 byteOffset = 0;
	};
}