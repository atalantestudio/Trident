// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct Extent2D {
		inline uint32 operator[](std::size_t index) const {
			ENGINE_ASSERT(index < 2, "Index overflow.");

			return pointer_cast<const uint32*>(this)[index];
		}

		inline uint32& operator[](std::size_t index) {
			ENGINE_ASSERT(index < 2, "Index overflow.");

			return pointer_cast<uint32*>(this)[index];
		}

		uint32 width;
		uint32 height;
	};

	struct Extent3D {
		Extent3D() = default;

		Extent3D(uint32 width, uint32 height, uint32 depth) :
			width(width),
			height(height),
			depth(depth)
		{}

		inline uint32 operator[](std::size_t index) const {
			ENGINE_ASSERT(index < 3, "Index overflow.");

			return pointer_cast<const uint32*>(this)[index];
		}

		inline uint32& operator[](std::size_t index) {
			ENGINE_ASSERT(index < 3, "Index overflow.");

			return pointer_cast<uint32*>(this)[index];
		}

		uint32 width;
		uint32 height;
		uint32 depth;
	};
}