#pragma once

namespace ProjectA {
	struct HuffmanCodeEntry {
		uint16 symbolIndex = 0;

		// Number of bits used for the symbol.
		uint16 bitLength = 0;
	};
}