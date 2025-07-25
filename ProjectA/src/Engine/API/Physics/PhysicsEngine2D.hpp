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
	class PhysicsEngine2D {
		private:
			static vec2<float32> calculateRelativeVelocity(const RigidBody2D& A, const RigidBody2D& B, const Contact2D& contact);

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

			void mergeContactManifolds(EntityContactPair2D& entityContactPair, const ContactManifold2D& contactManifold);

			// `inverseDeltaTime` is used to calculate the position bias.
			void precompute(EntityContactPair2D& pair, float32 inverseDeltaTime);

			void applyImpulse(EntityContactPair2D& pair);

		private:
			static constexpr uint32 SEQUENTIAL_IMPULSE_ITERATION_COUNT = 10;
			static constexpr float32 ALLOWED_PENETRATION = 0.01f;
			static constexpr float32 PENETRATION_BIAS = 0.2f;

			static inline const vec2<float32> G = {0.0f, -9.80665f};

		private:
			Scene2D* scene;
			std::vector<EntityPair> entityPairs;
			std::unordered_map<EntityPairKey, EntityContactPair2D> entityContactPairs;
	};
}