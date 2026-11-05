// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct PhysicsMaterial {
		/// Coulomb friction.
		float32 friction;

		/// Coefficient of restitution.
		float32 restitution;
	};
}