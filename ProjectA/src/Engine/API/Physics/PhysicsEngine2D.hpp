#pragma once

#include "Engine/API/Physics/EntityPair.hpp"
#include "Engine/API/Scene/Scene2D.hpp"

#ifdef BUILD_MODE_DEBUG
	#include "Engine/API/Physics/VisualizationData.hpp"
#endif

#define ACCUMULATE_IMPULSES

// TODO: Fix warm starting.
#define WARM_STARTING

namespace ProjectA {
	struct PhysicsEngine2DDescriptor {
		float64 deltaTime;

		#ifdef BUILD_MODE_DEBUG
			VisualizationData2D* visualizationData;
		#endif
	};

	// A simulated physics world.
	class PhysicsEngine2D {
		public:
			explicit PhysicsEngine2D(const PhysicsEngine2DDescriptor& descriptor);

			void registerScene(Scene2D& scene);

			void tick();

		private:
			void BroadPhase();
			void NarrowPhase();
			void ApplyForces();
			void UpdateManifolds();
			void ApplyImpulses();
			void Integrate();

			void mergeContactManifolds(EntityContactPair2D& entityContactPair, const ContactManifold2D& contactManifold);

		private:
			static constexpr uint32 SEQUENTIAL_IMPULSE_ITERATION_COUNT = 10;
			static constexpr float32 ALLOWED_PENETRATION = 0.01f;

			// Gravitational constant.
			static inline const vec2<float32> G = {0.0f, -9.80665f};

		private:
			Scene2D* scene;
			std::vector<EntityPair> entityPairs;
			std::unordered_map<EntityPairKey, EntityContactPair2D> entityContactPairs;
			float32 deltaTime;
			float32 inverseDeltaTime;
			float32 halfDeltaTime;

			#ifdef BUILD_MODE_DEBUG
				VisualizationData2D* const visualizationData = nullptr;
			#endif
	};
}