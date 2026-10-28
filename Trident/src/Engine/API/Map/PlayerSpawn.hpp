// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct PlayerSpawn {
		vec3<float32> position;

		/// Camera direction in Euler angles.
		vec3<float32> viewDirection;
	};
}