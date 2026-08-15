// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Process/Process.hpp"

namespace Trident {
	class ProcessSIMDProcess final : public Process {
		public:
			sint32 main(view<void*> arguments) override {
				static constexpr uint16 offsets[] = {0, 2, 5, 7, 9, 11, 14, 18, 22, 23, 29, 31};
				static constexpr view<uint16> offsetView = offsets;

				sequence<uint16> lengths(offsetView.count());

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
					const __m128i curr = _mm_load_si128(pointer_cast<const __m128i*>(&offsetView[regOffset]));
					const __m128i next = _mm_load_si128(pointer_cast<const __m128i*>(&offsetView[regOffset + 1]));
					const __m128i sub = _mm_sub_epi16(next, curr);

					_mm_store_si128(pointer_cast<__m128i*>(&lengths[regOffset]), sub);

					regOffset += 8;
				}

				while (regOffset < offsetView.count() - 1) {
					lengths[regOffset] = offsetView[regOffset + 1] - offsetView[regOffset];

					regOffset += 1;
				}

				return 0;
			}
	};
}