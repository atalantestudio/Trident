#pragma once

#include "Engine/API/Physics/CCD/CCDOutput.hpp"
#include "Engine/API/Physics/CCD/SeparationFunction.hpp"
#include "Engine/API/Physics/CCD/MinSeparationOutput.hpp"
#include "Engine/API/Physics/GJK2D.hpp"
#include "Engine/API/Scene/Scene2D.hpp"

namespace ProjectA {
	class Integrator {
		public:
			static void integrate(Scene2D& scene, GJK2DResult* gjkResult = nullptr);

		private:
			#ifdef BUILD_MODE_DEBUG
				static void printOutputState(CCDOutputState outputState);
			#endif

			static CCDOutput evaluateContinuousCollision(const Object2D& A, const Object2D& B, GJK2DResult* gjkResult);

			static Object2D copyObjectAt(Object2D A, float32 t);

			static SeparationFunction createSeparationFunction(const Object2D& A, const Object2D& B, const GJK2DResult& gjkResult);

			static MinSeparationOutput findMinSeparation(SeparationFunction separationFunction, const Object2D& A, const Object2D& B);

			static float32 evaluateSeparation(SeparationFunction separationFunction, const Object2D& A, const Object2D& B, MinSeparationOutput minSeparationOutput, float32 t);

			static void advanceSimulation(Object2D& A, float32 t);

			static void identifyClosestFeatures(
				SeparationType& separationType,
				const Object2D& objectA,
				const Object2D& objectB,
				const ClosestFeature2D& closestFeatureA,
				const ClosestFeature2D& closestFeatureB,
				const Object2D*& vertexObject,
				const Object2D*& edgeObject,
				const ClosestFeature2D*& vertexFeature,
				const ClosestFeature2D*& edgeFeature
			);

			static void identifyClosestFeatures(
				const Object2D& objectA,
				const Object2D& objectB,
				SeparationType separationType,
				const Object2D*& vertexObject,
				const Object2D*& edgeObject
			);
	};
}