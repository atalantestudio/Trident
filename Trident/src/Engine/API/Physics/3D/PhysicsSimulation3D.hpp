// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Descriptor.hpp"
#include "Engine/API/Physics/3D/ContactPair3D.hpp"
#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/ECS/EntityPair.hpp"

namespace Trident {
	#define ENABLE_PHYSICS_3D_IMPULSE_ACCUMULATION
	// #define ENABLE_PHYSICS_3D_WARM_STARTING

	class PhysicsSimulation3D;

	template<>
	struct Descriptor<PhysicsSimulation3D> {
		Scene3D* scene;
		float64 deltaTime;

		// TODO(debug)
		Entity playerEntity;
	};

	class PhysicsSimulation3D {
		private:
			static constexpr uint32 SEQUENTIAL_IMPULSE_ITERATION_COUNT = 10;
			static constexpr float32 ALLOWED_PENETRATION = 0.01f;

			inline static constexpr vec3<float32> G = {0.0f, -9.80665f, 0.0f};

		public:
			/// Constructs a physics simulation for the provided scene.
			explicit PhysicsSimulation3D(const Descriptor<PhysicsSimulation3D>& descriptor);

			/// Advances the simulation by a single tick.
			void tick();

		public:
			void broadPhase();
			void narrowPhase();
			void applyForces();
			void updateContactManifolds();
			void applyImpulses();

			/// NOTE: Uses Semi-Implicit Euler.
			void integrate();

		private:
			void mergeContactManifolds(ContactPair3D& contactPair, const ContactManifold3D& newContactManifold);

		public:
			std::map<EntityPair, ContactPair3D> contactPairs;

		private:
			Scene3D* scene;
			float32 deltaTime;
			float32 inverseDeltaTime;
			Entity playerEntity;
			std::vector<EntityPair> entityPairs;
	};
}