#pragma once

#include "Engine/API/Physics/CCD.hpp"
#include "Engine/API/Scene/Scene2D.hpp"

namespace ProjectA {
	class Integrator {
		public:
			static void integrate(Scene2D& scene, float32 fixedDeltaTime);

		private:
			static CCDOutput evaluateContinuousCollision(const Object2D& A, const Object2D& B);

			static void advanceSimulation(Object2D& A, float32 t);
	};
}