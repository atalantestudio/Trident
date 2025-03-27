#pragma once

#include "Engine/Compression/HuffmanCode.hpp"
#include "Engine/Filesystem/Stream/BitStream.hpp"

namespace ProjectA {
	class HuffmanCodeDictionary {
		public:
			HuffmanCodeDictionary(uint8 maxCodeLength);

			// Giving the bit lengths of the codes for each symbol of the alphabet in order
			// is sufficient to determine the actual codes.
			void compute(const uint8* codeLengths, uint16 symbolCount);

			uint16 decode(BitStream& stream) const;

		private:
			static uint32 reverseBits(uint32 value, uint8 bitCount);

		private:
			static constexpr uint8 MAX_CODE_LENGTH = 16;

		private:
			const uint8 maxCodeLength;
			chain<HuffmanCode> dictionary;
	};
}