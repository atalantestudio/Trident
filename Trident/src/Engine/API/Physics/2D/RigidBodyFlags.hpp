// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class RigidBodyFlags2D : uint8 {
		/// Don't include the body in simulation ticks.
		DISABLE_SIMULATION = BIT(0),

		/// Prevent linear movement along the X axis.
		LOCK_LINEAR_X_MOVEMENT = BIT(1),

		/// Prevent linear movement along the Y axis.
		LOCK_LINEAR_Y_MOVEMENT = BIT(2),

		/// Prevent rotational movement.
		LOCK_ANGULAR_MOVEMENT = BIT(3),
	};

	AS_FLAG(RigidBodyFlags2D);
}