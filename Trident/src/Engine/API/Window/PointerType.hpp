// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class PointerType : uint8 {
		// The platform-dependent default cursor.
		// Typically an arrow.
		DEFAULT,

		// Indicates that the hovered element can be interacted with.
		// Typically an image of a pointing hand.
		INTERACTIVE,
	};
}