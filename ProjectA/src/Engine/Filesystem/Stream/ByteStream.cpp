#include "pch.hpp"

#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace ProjectA {
	ByteStream::ByteStream(const uint8* stream, uint64 size) :
		stream(stream),
		size(size)
	{}

	uint64 ByteStream::getByteOffset() const {
		return byteOffset;
	}

	const uint8* ByteStream::getBytePosition() const {
		return stream + byteOffset;
	}

	uint64 ByteStream::getRemainingByteSize() const {
		return size - byteOffset;
	}

	void ByteStream::seek(uint64 offset) {
		ENGINE_ASSERT(offset <= size, "Cannot seek more bytes than there is in the stream.", offset, size);

		byteOffset = offset;
	}

	void ByteStream::jump(uint64 count) {
		const uint64 remainingByteSize = getRemainingByteSize();

		ENGINE_ASSERT(count <= remainingByteSize, "Attempted to jump [] bytes, but only [] were remaining.", count, remainingByteSize);

		byteOffset += count;
	}
}