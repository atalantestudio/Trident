#pragma once

namespace ProjectA {
	class ByteStream {
		public:
			template<typename T>
			static constexpr T byteswap(T value) {
				return value;
			}

			template<>
			static uint16 byteswap(uint16 value) {
				return _byteswap_ushort(value);
			}

			template<>
			static uint32 byteswap(uint32 value) {
				return _byteswap_ulong(value);
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

			// Advances by the provided byte count.
			// Usage:
			// - The byte count must be <= the remaining bytes in the stream.
			void jump(uint64 byteCount);

			// Returns the next bytes from the stream without updating the offsets.
			// Usage:
			// - The byte size of T must be <= the remaining bytes in the stream.
			template<typename T>
			T peek(bool swapEndianness = false) const {
				const uint64 remainingByteSize = getRemainingByteSize();

				ENGINE_ASSERT(sizeof(T) <= remainingByteSize, "Attempted to peek [] bytes, but only [] were remaining.", sizeof(T), remainingByteSize);

				T value = *(T*)(stream + byteOffset);

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