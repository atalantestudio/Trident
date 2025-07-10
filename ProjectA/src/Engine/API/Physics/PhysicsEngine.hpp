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
	};

	class PhysicsEngine {
		public:
			void update(Scene2D& scene, GJK2DResult* gjkResult = nullptr);

		private:
			void broadPhase();

			void narrowPhase(GJK2DResult* gjkResult);

			void integrate();

			CCDOutput evaluateContinuousCollision(Entity A, Entity B, GJK2DResult* gjkResult);

			SeparationFunction createSeparationFunction(const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, const GJK2DResult& gjkResult);

			MinSeparationOutput findMinSeparation(SeparationFunction separationFunction, const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B);

			// Arguments:
			// - Entity geometries
			// - Entity transforms at time t
			// - Entity rotations at time t
			float32 evaluateSeparation(SeparationFunction separationFunction, Entity A, Entity B, MinSeparationOutput minSeparationOutput, float32 t);

			void identifyClosestFeatures(
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

			void identifyClosestFeatures(const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, SeparationType separationType, const PhysicalObjectCopy2D*& vertexObject, const PhysicalObjectCopy2D*& edgeObject);

			void advanceSimulation(Entity entity, float32 t);

		private:
			Scene2D* scene;
			std::vector<CollisionPair> pairs;
			std::vector<float32> safeAdvanceTimes;
			std::vector<ImpulseData> impulses;
	};
}