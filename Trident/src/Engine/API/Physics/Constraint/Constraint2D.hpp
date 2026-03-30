// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/ECS/ECS.hpp"

namespace Trident {
	// TODO: Replace virtual properties by LUT.
	class Constraint2D {
		public:
			virtual void update(ECS* ecs, float32 deltaTime) const = 0;
	};
}