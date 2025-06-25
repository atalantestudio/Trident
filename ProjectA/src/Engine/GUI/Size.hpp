#pragma once

#include "Engine/GUI/FillMode.hpp"

namespace ProjectA {
	class Size1D {
		public:
			Size1D() = default;

			Size1D(uint32 fillModeOrPixels) :
				value(fillModeOrPixels)
			{
				if (fillModeOrPixels < static_cast<uint32>(-2)) {
					fillMode = FillMode::FIXED;
					value = fillModeOrPixels;
				}
				else {
					fillMode = static_cast<FillMode>(fillModeOrPixels + 3);
					value = 0;
				}
			}

			operator uint32() const {
				return value;
			}

		public:
			FillMode fillMode = FillMode::FIXED;
			uint32 value = 0;
	};

	struct Size2D {
		Size1D width;
		Size1D height;

		Size1D operator [](std::size_t index) const {
			// TODO: Assert.
			return pointer_cast_const<const Size1D*>(this)[index];
		}

		Size1D& operator [](std::size_t index) {
			// TODO: Assert.
			return pointer_cast<Size1D*>(this)[index];
		}
	};
}