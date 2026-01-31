// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	// TODO: Those presets are from https://code.tutsplus.com/how-to-create-a-custom-2d-physics-engine-the-core-engine--gamedev-7493t.
	// | Type       | Density | Restitution |
	// | ---------- | ------- | ----------- |
	// | Rock       | 0.6     | 0.1         |
	// | Wood       | 0.3     | 0.2         |
	// | Metal      | 1.2     | 0.05        |
	// | BouncyBall | 0.3     | 0.8         |
	// | SuperBall  | 0.3     | 0.95        |
	// | Pillow     | 0.1     | 0.2         |
	// | Static     | 0.0     | 0.4         |

	struct PhysicsMaterial {
		// Mass is assumed to be uniformly distributed.
		float32 density = 1.0f;

		// Coefficient of restitution. Overridden when the gravity is the only force.
		float32 restitution = 1.0f;
	};
}