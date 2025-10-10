#pragma once

#include "Engine/Filesystem/Stream/Stream.hpp"
#include "Engine/Filesystem/Stream/StreamOperation.hpp"

namespace ProjectA {
	template<StreamOperation StreamOperation>
	class ByteStream;

	template<>
	class ByteStream<StreamOperation::READ> : public Stream {
		public:
			explicit ByteStream(const uint8* data, uint64 size, Endianness endianness) :
				Stream(size, endianness),
				data(data)
			{}

			const uint8* getBytePosition() const {
				return data + byteOffset;
			}

			// Returns the next bytes from the stream without updating the offsets.
			// The byte size of T must be <= the remaining bytes in the stream.
			template<typename T>
			T peek(bool swapEndianness = false) const {
				const uint64 remainingByteSize = getRemainingSize();

				ENGINE_ASSERT(sizeof(T) <= remainingByteSize, "Attempted to peek [] bytes, but only [] were remaining.", sizeof(T), remainingByteSize);

				T value = *pointer_cast<const T*>(data + byteOffset);

				if (static_cast<uint8>(endianness) + swapEndianness ^ static_cast<uint8>(Platform::getEndianness())) {
					value = byteswap<T>(value);
				}

				return value;
			}

			// TODO: Allow entire structs to be read using ByteStream,
			//       e.g. const MyStruct myStruct = stream.read<MyStruct>().
			// 
			// Casts to T and returns the next bytes from the stream.
			// The byte size of T must be <= the remaining bytes in the stream.
			template<typename T>
			T read(bool swapEndianness = false) {
				const T value = peek<T>(swapEndianness);

				jump(sizeof(T));

				return value;
			}

		public:
			const uint8* data;
	};

	template<>
	class ByteStream<StreamOperation::WRITE> : public Stream {
		public:
			explicit ByteStream(uint64 size, Endianness endianness) :
				Stream(size, endianness),
				data(new uint8[size]())
			{}

			~ByteStream() {
				delete[] data;
			}

			const uint8* getBytePosition() const {
				return data + byteOffset;
			}

			uint8* getBytePosition() {
				return data + byteOffset;
			}

			template<typename T>
			void write(T value, bool swapEndianness = false) {
				const uint64 remainingByteSize = getRemainingSize();

				ENGINE_ASSERT(sizeof(T) <= remainingByteSize, "Attempted to write [] bytes, but only [] were remaining.", sizeof(T), remainingByteSize);

				if (swapEndianness) {
					*pointer_cast<T*>(&data[byteOffset]) = byteswap<T>(value);
				}
				else {
					*pointer_cast<T*>(&data[byteOffset]) = value;
				}

				jump(sizeof(T));
			}

		public:
			uint8* data;
	};
}