// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class CullMode : uint8 {
		/// Specifies that no triangles are discarded.
		NONE,

		/// Specifies that front-facing triangles are discarded.
		FRONT,

		/// Specifies that back-facing triangles are discarded.
		BACK,

		count,
	};
}