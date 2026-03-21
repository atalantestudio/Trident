// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Scene/Scene2D.hpp"

namespace Trident {
	class Constraint2D {
		public:
			virtual void update(Scene2D* scene, float32 deltaTime) const = 0;
	};
}