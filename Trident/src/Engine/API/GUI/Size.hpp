// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/FillMode.hpp"

namespace Trident {
	class Size1D {
		public:
			Size1D() = default;

			Size1D(float32 fillModeOrPixels) {
				if (fillModeOrPixels >= 0.0f) {
					value = fillModeOrPixels;
				}
				else {
					fillMode = static_cast<FillMode>(fillModeOrPixels + 2);
				}
			}

			inline operator float32() const {
				return value;
			}

		public:
			FillMode fillMode = FillMode::FIXED;
			float32 value = 0.0f;
	};

	struct Size2D {
		Size1D width;
		Size1D height;

		inline Size1D operator[](std::size_t index) const {
			ENGINE_ASSERT(index < 2, "Index overflow.");

			return pointer_cast<const Size1D*>(this)[index];
		}

		inline Size1D& operator[](std::size_t index) {
			ENGINE_ASSERT(index < 2, "Index overflow.");

			return pointer_cast<Size1D*>(this)[index];
		}
	};
}