#pragma once

#include "Engine/Filesystem/Compression/HuffmanCodeEntry.hpp"
#include "Engine/Filesystem/Stream/BitStream.hpp"

namespace ProjectA {
	class HuffmanCodeDictionary {
		public:
			static uint32 reverseBits(uint32 value, uint32 bitCount);

		public:
			HuffmanCodeDictionary(uint8 maxCodeBitLength);

			// Giving the bit lengths of the codes for each symbol of the alphabet in order
			// is sufficient to determine the actual codes.
			void compute(const uint16* codeLengths, uint16 symbolCount);

			uint16 decode(BitStream& stream) const;

		private:
			static constexpr uint8 MAX_CODE_BIT_LENGTH = 16;

		private:
			uint8 maxCodeBitLength;
			chain<HuffmanCodeEntry> codeEntries;
	};
}