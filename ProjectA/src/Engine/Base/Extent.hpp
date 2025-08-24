#pragma once

namespace ProjectA {
	struct Extent2D {
		uint32 width;
		uint32 height;

		uint32 operator [](std::size_t index) const {
			ENGINE_ASSERT(index < 2, "Index overflow.");

			return pointer_cast<const uint32*>(this)[index];
		}

		uint32& operator [](std::size_t index) {
			ENGINE_ASSERT(index < 2, "Index overflow.");

			return pointer_cast<uint32*>(this)[index];
		}
	};

	struct Extent3D {
		uint32 width;
		uint32 height;
		uint32 depth;

		uint32 operator [](std::size_t index) const {
			ENGINE_ASSERT(index < 3, "Index overflow.");

			return pointer_cast<const uint32*>(this)[index];
		}

		uint32& operator [](std::size_t index) {
			ENGINE_ASSERT(index < 3, "Index overflow.");

			return pointer_cast<uint32*>(this)[index];
		}
	};
}