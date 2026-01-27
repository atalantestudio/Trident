// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Window/PointerType.hpp"

namespace Trident {
	inline LPWSTR getWin32CursorName(PointerType pointerType) {
		switch (pointerType) {
			case PointerType::DEFAULT: return IDC_ARROW;
			case PointerType::INTERACTIVE: return IDC_HAND;
		}

		DEBUG_ASSERT(false);

		return nullptr;
	}
}