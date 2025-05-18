#include "pch.hpp"

#include "Engine/API/Physics/Integrator.hpp"

namespace ProjectA {
	void Integrator::integrate(Scene2D& scene, float32 fixedDeltaTime) {
		for (uint32 i = 0; i < scene.getObjectCount() - 1; i += 1) {
			for (uint32 j = i + 1; j < scene.getObjectCount(); j += 1) {
				Object2D& A = scene[i];
				Object2D& B = scene[j];

				// const CCDOutput output = evaluateContinuousCollision(A, B);

				advanceSimulation(A, fixedDeltaTime);
				advanceSimulation(B, fixedDeltaTime);

				// TODO: Resolve possible collisions.
			}
		}
	}

	CCDOutput Integrator::evaluateContinuousCollision(const Object2D& A, const Object2D& B) {
		CCDOutput output{};

		// TODO
		output.fraction = 1.0f;

		return output;
	}

	void Integrator::advanceSimulation(Object2D& A, float32 t) {
		A.position += A.linearVelocity * t;
		A.rotation += A.angularVelocity * t;

		A.linearVelocity += A.linearAcceleration * t;
		A.angularVelocity += A.angularAcceleration * t;

		A.updateTransform();
	}
}