#pragma once

#include "Engine/API/Physics/EntityPair.hpp"
#include "Engine/API/Scene/Scene2D.hpp"

#ifdef BUILD_MODE_DEBUG
	#include "Engine/API/Physics/VisualizationData.hpp"
#endif

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

namespace ProjectA {
	class PhysicsEngine2D {
		public:
			void registerScene(Scene2D& scene);

			void tick(
				float32 deltaTime,
				float32 inverseDeltaTime
				#ifdef BUILD_MODE_DEBUG
					, VisualizationData2D* visualizationData = {}
				#endif
			);

		private:
			void BroadPhase();

			void NarrowPhase(
				#ifdef BUILD_MODE_DEBUG
					VisualizationData2D* visualizationData
				#endif
			);

			// `inverseDeltaTime` is used to calculate the position bias.
			void precompute(EntityContactPair2D& pair, float32 inverseDeltaTime);

			void applyImpulse(const EntityContactPair2D& pair);

		private:
			static constexpr uint32 SEQUENTIAL_IMPULSE_ITERATION_COUNT = 10;

			static inline const vec2<float32> G = {0.0f, -9.81f};

		private:
			Scene2D* scene;

			std::vector<EntityPair> entityPairs;
			std::vector<EntityContactPair2D> entityContactPairs;
	};
}