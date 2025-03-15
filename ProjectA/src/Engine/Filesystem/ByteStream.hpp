#pragma once

namespace ProjectA {
	class ByteStream {
		public:
			ByteStream(const uint8* stream, uint64 size);

			// Advances by the provided byte count.
			// Usage:
			// - The byte count must be <= the remaining bytes in the stream.
			void jump(uint64 byteCount);

			// Returns the next bytes from the stream without updating the offsets.
			// Usage:
			// - The byte size of T must be <= the remaining bytes in the stream.
			template<typename T>
			T peek(bool swapEndianness = false) const {
				ENGINE_ASSERT(sizeof(T) <= getRemainingByteSize(), "ByteStream: Attempted to peek [] bytes, but only [] were left.", sizeof(T), getRemainingByteSize());

				T value = *(T*)(stream + byteOffset);

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
				ENGINE_ASSERT(sizeof(T) <= getRemainingByteSize(), "ByteStream: Attempted to read [] bytes, but only [] were left.", sizeof(T), getRemainingByteSize());

				const T value = peek<T>(swapEndianness);

				jump(sizeof(T));

				return value;
			}

			uint64 getRemainingByteSize() const;

		private:
			const uint8* stream;

			// Size of the stream in bytes.
			uint64 size;

			// Offset of the currently read byte.
			uint64 byteOffset = 0;
	};
}