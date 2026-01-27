// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class MouseButton : uint8 {
		BUTTON_0 = 0,
		BUTTON_1 = 1,
		BUTTON_2 = 2,
		BUTTON_3 = 3,
		BUTTON_4 = 4,
		BUTTON_5 = 5,
		BUTTON_6 = 6,
		BUTTON_7 = 7,

		// Aliases.
		LAST = BUTTON_7,
		LEFT = BUTTON_0,
		RIGHT = BUTTON_1,
		MIDDLE = BUTTON_2,

		count,
	};

	AS_PRINTABLE(MouseButton);
}