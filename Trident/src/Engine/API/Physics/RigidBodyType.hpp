// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class RigidBodyType : uint8 {
		/// Indicates a body with a mass of 0 that must be manually moved.
		STATIC,

		/// Indicates a body with a mass of 0 whose forces must be applied manually.
		KINEMATIC,

		/// Indicates a body with non-zero mass moved by the solver.
		DYNAMIC,

		count,
	};
}