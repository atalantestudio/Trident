// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class GUIComponentType : uint8 {
		/// Container components.
		DEFAULT,

		/// Single-line and multi-line static text components.
		TEXT,

		/// Single-line text input components.
		TEXT_INPUT,

		/// Vertical list components.
		VERTICAL_LIST,
	};
}