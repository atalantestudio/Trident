// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class RigidBodyType : uint8 {
		/// A body with a mass of 0 that must be manually moved.
		STATIC,

		/// A body with a mass of 0 whose forces must be applied manually.
		KINEMATIC,

		/// A body with non-zero mass moved by the solver.
		DYNAMIC,

		count,
	};
}