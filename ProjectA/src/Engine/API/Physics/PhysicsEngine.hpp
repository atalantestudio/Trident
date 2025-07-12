#pragma once

#include "Engine/API/Physics/CCD/CCDOutput.hpp"
#include "Engine/API/Physics/CCD/SeparationFunction.hpp"
#include "Engine/API/Physics/CCD/MinSeparationOutput.hpp"
#include "Engine/API/Physics/CollisionPair.hpp"
#include "Engine/API/Physics/GJK2D.hpp"
#include "Engine/API/Scene/Scene2D.hpp"

namespace ProjectA {
	struct ImpulseData {
		Entity entity;

		vec2<float32> impulse;

		// Vector from the COM to the contact point.
		vec2<float32> r;
	};

	class PhysicsEngine {
		public:
			void update(Scene2D& scene, GJK2DResult* gjkResult = nullptr);

		private:
			void broadPhase();

			void narrowPhase(GJK2DResult* gjkResult);

			void integrate();

			CCDOutput evaluateContinuousCollision(Entity A, Entity B, GJK2DResult* gjkResult);

			void advanceSimulation(Entity entity, float32 t);

		private:
			Scene2D* scene;
			std::vector<CollisionPair> pairs;
			std::vector<float32> safeAdvanceTimes;
			std::vector<ImpulseData> impulses;
	};
}