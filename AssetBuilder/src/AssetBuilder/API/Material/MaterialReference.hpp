// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	/// 32-bit unsigned integer separated in 2,
	/// with the 16 MSB representing the material shader index
	/// and the 16 LSB representing the material index.
	typedef uint32 MaterialReference;
}