#pragma once

#include "Engine/GUI/FillMode.hpp"

namespace ProjectA {
	class Size1D {
		public:
			Size1D() = default;

			Size1D(float32 fillModeOrPixels) {
				if (fillModeOrPixels >= 0.0f) {
					value = fillModeOrPixels;
				}
				else {
					// TODO: Remove sint32 cast?
					fillMode = static_cast<FillMode>(static_cast<sint32>(fillModeOrPixels) + 2);
				}
			}

			operator float32() const {
				return value;
			}

		public:
			FillMode fillMode = FillMode::FIXED;
			float32 value = 0.0f;
	};

	struct Size2D {
		Size1D width;
		Size1D height;

		Size1D operator [](std::size_t index) const {
			// TODO: Assert.
			return pointer_cast<const Size1D*>(this)[index];
		}

		Size1D& operator [](std::size_t index) {
			// TODO: Assert.
			return pointer_cast<Size1D*>(this)[index];
		}

		operator svec2<float32>() const {
			return {width.value, height.value};
		}
	};
}