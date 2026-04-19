// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Descriptor.hpp"
#include "Engine/API/Physics/2D/ContactPair2D.hpp"
#include "Engine/API/Scene/Scene2D.hpp"
#include "Engine/ECS/EntityPair.hpp"

namespace Trident {
	#define ENABLE_PHYSICS_2D_IMPULSE_ACCUMULATION
	#define ENABLE_PHYSICS_2D_WARM_STARTING

	class PhysicsSimulation2D;

	template<>
	struct Descriptor<PhysicsSimulation2D> {
		Scene2D* scene;
		float64 deltaTime;
	};

	/// A simulated physics world.
	class PhysicsSimulation2D {
		private:
			static constexpr uint32 SEQUENTIAL_IMPULSE_ITERATION_COUNT = 10;
			static constexpr float32 ALLOWED_PENETRATION = 0.01f;

			inline static const vec2<float32> G = {0.0f, -9.80665f};

		private:
			static void mergeContactManifolds(ContactPair2D& contactPair, const ContactManifold2D& newContactManifold);

		public:
			/// Constructs a physics simulation for the provided scene.
			explicit PhysicsSimulation2D(const Descriptor<PhysicsSimulation2D>& descriptor);

			/// Advances the simulation by a single tick.
			void tick();

		private:
			const Hull2D& getHull(Entity entity) const;

			void broadPhase();
			void narrowPhase();
			void applyForces();
			void updateContactManifolds();
			void updateJoints();
			void applyImpulses();

			/// NOTE: Uses Semi-Implicit Euler.
			void integrate();

		private:
			Scene2D* scene;

			float32 deltaTime;
			float32 inverseDeltaTime;

			// TODO: Use.
			// float32 halfDeltaTime;

			std::vector<EntityPair> entityPairs;
			std::map<EntityPair, ContactPair2D> contactPairs;
	};
}