// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/ID.hpp"

namespace Trident {
	struct TextStyle {
		Color color = 0xffffffff;
		FontID font;
		float32 fontSize;

		#if defined(BUILD_MODE_DEBUG)
			view<char8> debugName;
		#endif
	};
}