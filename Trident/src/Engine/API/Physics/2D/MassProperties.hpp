// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct MassProperties2D {
		/// Mass in kg.
		float32 mass;

		/// Inertia about the center of mass.
		float32 inertia;
	};
}