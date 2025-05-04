#pragma once

namespace ProjectA {
	struct GUISize {
		sint16 width;
		sint16 height;

		sint16 operator [](std::size_t index) const {
			ENGINE_ASSERT(index < 2, "Index overflow.");

			switch (index) {
				default:
				case 0:
					return width;
				case 1:
					return height;
			}
		}
	};
}