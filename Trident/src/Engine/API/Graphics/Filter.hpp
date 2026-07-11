// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	/// Texture lookup filters.
	enum class Filter : uint8 {
		/// Nearest filtering.
		NEAREST,

		/// Linear filtering.
		LINEAR,

		count,
	};
}