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

			static CCDOutput evaluateContinuousCollision(const Scene2D& scene, Entity A, Entity B, GJK2DResult* gjkResult);

			static SeparationFunction createSeparationFunction(const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, const GJK2DResult& gjkResult);

			static MinSeparationOutput findMinSeparation(SeparationFunction separationFunction, const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B);

			// Arguments:
			// - Entity geometries
			// - Entity transforms at time t
			// - Entity rotations at time t
			static float32 evaluateSeparation(SeparationFunction separationFunction, const Scene2D& scene, Entity A, Entity B, MinSeparationOutput minSeparationOutput, float32 t);

			static void advanceSimulation(Scene2D& scene, Entity A, float32 t);

			static void identifyClosestFeatures(
				SeparationType& separationType,
				const PhysicalObjectCopy2D& A,
				const PhysicalObjectCopy2D& B,
				const ClosestFeature2D& closestFeatureA,
				const ClosestFeature2D& closestFeatureB,
				const PhysicalObjectCopy2D*& vertexObject,
				const PhysicalObjectCopy2D*& edgeObject,
				const ClosestFeature2D*& vertexFeature,
				const ClosestFeature2D*& edgeFeature
			);

			static void identifyClosestFeatures(
				const PhysicalObjectCopy2D& A,
				const PhysicalObjectCopy2D& B,
				SeparationType separationType,
				const PhysicalObjectCopy2D*& vertexObject,
				const PhysicalObjectCopy2D*& edgeObject
			);
	};
}