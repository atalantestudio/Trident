#include "pch.hpp"

#include "Engine/Filesystem/ByteStream.hpp"

namespace ProjectA {
	ByteStream::ByteStream(const uint8* stream, uint64 size) :
		stream(stream),
		size(size)
	{}

	void ByteStream::jump(uint64 byteCount) {
		ENGINE_ASSERT(byteCount <= getRemainingByteSize(), "ByteStream: Attempted to jump [] bytes, but only [] were left.", byteCount, getRemainingByteSize());

		byteOffset += byteCount;
	}

	uint64 ByteStream::getRemainingByteSize() const {
		return size - byteOffset;
	}
}