// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/EventListeners.hpp"
#include "Engine/API/GUI/ID.hpp"

namespace Trident {
	struct TextStyle {
		Color color = 0xffffffff;
		FontID font;
		float32 fontSize;
		const EventListeners* eventListeners;

		// TODO (debug-only)
		view<char8> debugName;
	};
}