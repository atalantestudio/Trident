#pragma once

#include "Engine/API/Physics/Arbiter.hpp"
#include "Engine/API/Physics/CCD/MinSeparationOutput.hpp"
#include "Engine/API/Physics/CCD/SeparationFunction.hpp"
#include "Engine/API/Physics/CCD/TimeOfImpact.hpp"
#include "Engine/API/Physics/CollisionPair.hpp"
#include "Engine/API/Scene/Scene2D.hpp"

// TODO
// https://www.gamedevs.org/uploads/anatomy-of-physics-engine.pdf
// 
// Collision data:
// - Collision shapes
// - Object AABBs
// - Overlapping pairs
// - Contacts (manifold)
// 
// Dynamics data:
// - Motion state, transforms, velocities
// - Rigid bodies, mass, inertia
// - Constraints, contacts, joints
// 
// Forward dynamics:
// - Apply gravity
// - Predict transforms
// 
// Collision detection:
// - Compute AABBS
// - Detect pairs
// - Compute contacts
// 
// Forward dynamics:
// - Setup constraints
// - Solve constraints
// - Integrate positions
// 
// 
// 
// Sequential Impulses:
// 1. Calculate relative velocity and penetration error for each pair
// 2. Apply correction impulse
// 3. Repeat ~10 times for all pairs

namespace ProjectA {
	struct VisualizationData2D;

	class PhysicsEngine {
		public:
			void setScene(Scene2D& scene);

			void tick(float32 deltaTime, VisualizationData2D* visualizationData = {});

		private:
			std::vector<CollisionPair> BroadPhase() const;

			std::vector<Arbiter2D> NarrowPhase(const std::vector<CollisionPair>& collisionPairs, VisualizationData2D* visualizationData) const;

		private:
			static constexpr uint32 SEQUENTIAL_IMPULSE_ITERATION_COUNT = 10;

			static inline const vec2<float32> G = {0.0f, -9.81f};

		private:
			Scene2D* scene;
	};
}