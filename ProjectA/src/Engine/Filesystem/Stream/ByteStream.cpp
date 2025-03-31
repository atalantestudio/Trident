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

	void ByteStream::jump(uint64 byteCount) {
		const uint64 remainingByteSize = getRemainingByteSize();

		ENGINE_ASSERT(byteCount <= remainingByteSize, "Attempted to jump [] bytes, but only [] were remaining.", byteCount, remainingByteSize);

		byteOffset += byteCount;
	}

	void ByteStream::reset() {
		byteOffset = 0;
	}
}