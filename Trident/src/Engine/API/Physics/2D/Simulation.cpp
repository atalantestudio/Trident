// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Simulation.hpp"

namespace Trident {
	PhysicsSimulation2D createSimulation(const PhysicsSimulation2DDescriptor& descriptor) {
		PhysicsSimulation2D simulation;
		simulation.scene = descriptor.scene;
		simulation.deltaTime = static_cast<float32>(descriptor.deltaTime);
		simulation.inverseDeltaTime = static_cast<float32>(1.0 / descriptor.deltaTime);
		simulation.gravity = descriptor.gravity;

		// TODO

		return simulation;
	}
}