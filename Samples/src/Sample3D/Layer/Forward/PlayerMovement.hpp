// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum PlayerMovementState : uint8 {
		AIR,
		GROUND,
	};

	struct PlayerMovement {
		PlayerMovementState state;
	};
}