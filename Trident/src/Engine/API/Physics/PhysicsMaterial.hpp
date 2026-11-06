// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct PhysicsMaterial {
		/// Coulomb friction.
		float32 friction;

		/// The coefficient of restitution.
		/// See https://en.wikipedia.org/wiki/Coefficient_of_restitution.
		float32 restitution;
	};
}