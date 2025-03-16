#pragma once

#include "Engine/Filesystem/BitStream.hpp"
#include "Engine/Filesystem/HuffmanCodeEntry.hpp"

namespace ProjectA {
	class HuffmanCodeDictionary {
		public:
			static uint32 reverseBits(uint32 value, uint8 count);

		public:
			HuffmanCodeDictionary(uint8 maxCodeBitLength);

			~HuffmanCodeDictionary();

			// Giving the bit lengths of the codes for each symbol of the alphabet in order
			// is sufficient to determine the actual codes.
			// NOTE: The symbol count can technically be uint16.
			void compute(const uint16* codeLengths, uint32 symbolCount);

			uint16 decode(BitStream& stream) const;

		private:
			static constexpr uint8 MAX_CODE_BIT_LENGTH = 16;

		private:
			uint8 maxCodeBitLength;
			uint32 codeEntryCount = 0;
			HuffmanCodeEntry* codeEntries = nullptr;
	};
}