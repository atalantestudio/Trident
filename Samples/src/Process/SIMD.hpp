// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class ProcessSIMDProcess final : public Process {
		public:
			sint32 main(view<void*> arguments) override {
				sequence<uint16> offsets = {0, 2, 5, 7, 9, 11, 14, 18, 22, 23, 29, 31};
				sequence<uint16> lengths(offsets.count());

				const uint64 regCount = lengths.count() / 8;

				uint64 regOffset = 0;

				// TODO(lyah/register)
				/* while (regOffset < regCount * 8) {
					const reg<8, uint16> current(&glyphOffsets[regOffset]);
					const reg<8, uint16> next(&glyphOffsets[regOffset + 1]);

					store(&glyphLengths[regOffset], next - current);

					regOffset += 8;
				} */

				while (regOffset < regCount * 8) {
					const __m128i curr = _mm_load_si128(pointer_cast<const __m128i*>(&offsets[regOffset]));
					const __m128i next = _mm_load_si128(pointer_cast<const __m128i*>(&offsets[regOffset + 1]));
					const __m128i sub = _mm_sub_epi16(next, curr);

					_mm_store_si128(pointer_cast<__m128i*>(&lengths[regOffset]), sub);

					regOffset += 8;
				}

				while (regOffset < offsets.count() - 1) {
					lengths[regOffset] = offsets[regOffset + 1] - offsets[regOffset];

					regOffset += 1;
				}

				return 0;
			}
	};
}