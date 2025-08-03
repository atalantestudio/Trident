#pragma once

#include "Engine/Filesystem/Stream/BitStream.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace ProjectA {
	class Stream {
		public:
			explicit Stream(const sequence<uint8>& buffer) :
				buffer(buffer)
			{}

			const sequence<uint8>& getBuffer() const {
				return buffer;
			}

			BitStream asBitStream() const {
				return BitStream(buffer, buffer.count);
			}

			ByteStream asByteStream() const {
				return ByteStream(buffer, buffer.count);
			}

		private:
			const sequence<uint8> buffer;
	};
}