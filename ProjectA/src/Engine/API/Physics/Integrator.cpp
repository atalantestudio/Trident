#include "pch.hpp"

#include "Engine/API/Physics/GJK2D.hpp"
#include "Engine/API/Physics/Integrator.hpp"

namespace ProjectA {
	static constexpr uint32 DISTANCE_ITERATION_LIMIT = 20;
	static constexpr uint32 PUSH_BACK_ITERATION_LIMIT = 8;
	static constexpr uint32 ROOT_FINDER_ITERATION_LIMIT = 50;

	// Enable/disable continuous collision detection.
	#define DEBUG_ENABLE_CCD

	void Integrator::integrate(Scene2D& scene, GJK2DResult* gjkResult) {
		for (Entity A = 0; A < scene.getPhysicalObjectEntityCount() - 1; A += 1) {
			for (Entity B = A + 1; B < scene.getPhysicalObjectEntityCount(); B += 1) {
				float32 toi = 1.0f;

				#ifdef DEBUG_ENABLE_CCD
					const CCDOutput output = evaluateContinuousCollision(scene, A, B, gjkResult);

					#ifdef BUILD_MODE_DEBUG
						printOutputState(output.state);
					#endif

					toi = output.fraction;
				#endif

				advanceSimulation(scene, A, toi);
				advanceSimulation(scene, B, toi);

				// https://code.tutsplus.com/how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331t
				if (output.state == CCDOutputState::HIT) {
					ref<RigidBody2D>& rigidBodyA = std::static_pointer_cast<RigidBody2D>(scene.getBody(A));
					ref<RigidBody2D>& rigidBodyB = std::static_pointer_cast<RigidBody2D>(scene.getBody(B));
					const ref<Material>& materialA = scene.getMaterial(A);
					const ref<Material>& materialB = scene.getMaterial(B);

					const vec2<float32> collisionNormal = normalized(rigidBodyB->position - rigidBodyA->position);
					const vec2<float32> relativeLinearVelocity = rigidBodyB->linearVelocity - rigidBodyA->linearVelocity;
					const float32 velocityAlongNormal = dot(relativeLinearVelocity, collisionNormal);

					if (velocityAlongNormal > 0.0f) {
						// These entities are separating; continue to the next entities.
						continue;
					}

					// TODO: lyah::min
					// Calculate coefficient of restitution.
					const float32 e = std::min(materialA->coefficientOfRestitution, materialB->coefficientOfRestitution);

					// Calculate linear impulse.
					const float32 j = (-(1 + e) * velocityAlongNormal) / (rigidBodyA->inverseMass + rigidBodyB->inverseMass);
					const vec2<float32> impulse = collisionNormal * j;

					// Apply linear impulse.
					rigidBodyA->linearVelocity -= impulse * rigidBodyA->inverseMass;
					rigidBodyB->linearVelocity += impulse * rigidBodyB->inverseMass;

					// TODO: t < 0.1 does not move the entities.
					advanceSimulation(scene, A, std::max(0.1f, 1.0f - toi));
					advanceSimulation(scene, B, std::max(0.1f, 1.0f - toi));
				}
			}
		}
	}

	#ifdef BUILD_MODE_DEBUG
		void Integrator::printOutputState(CCDOutputState outputState) {
			switch (outputState) {
				case CCDOutputState::UNDETERMINED:
					ENGINE_ERROR("Integrator: CCD state is undetermined.");

					break;
				case CCDOutputState::UNRESOLVED_ROOT:
					ENGINE_WARN("Integrator: Root finder ran out of iterations.");

					break;
				default:
					break;
			}
		}
	#endif

	// Bilateral Advancement algorithm implementation.
	// Evaluates continuous collision between A and B.
	// If `gjkResult` is not null, the result of the last GJK call is written to it.
	// References:
	// 
	// - Erin Catto - Continuous Collision - GDC 2013
	//   https://box2d.org/files/ErinCatto_ContinuousCollision_GDC2013.pdf (54 pages)
	//   https://ubm-twvideo01.s3.amazonaws.com/o1/vault/gdc2013/slides/824737Catto_Erin_PhysicsForGame.pdf (67 pages)
	//   https://www.youtube.com/watch?v=7_nKOET6zwI
	// - Nilson Souto - Video Game Physics Tutorial - Part II: Collision Detection for Solid Objects
	//   https://www.toptal.com/game/video-game-physics-part-ii-collision-detection-for-solid-objects
	// 
	CCDOutput Integrator::evaluateContinuousCollision(const Scene2D& scene, Entity A, Entity B, GJK2DResult* gjkResult) {
		static constexpr float32 TOLERANCE = 0.25f * 0.005f;
		static constexpr float32 TARGET = 0.005f;

		ENGINE_STATIC_ASSERT(TARGET > TOLERANCE);

		CCDOutput output{};

		output.state = CCDOutputState::UNDETERMINED;
		output.fraction = 0.0f;

		float32 t0 = 0.0f;

		const PhysicalObjectCopy2D baseA = copyPhysicalObject(scene, A);
		const PhysicalObjectCopy2D baseB = copyPhysicalObject(scene, B);

		uint32 distanceIteration = 0;

		while (distanceIteration < DISTANCE_ITERATION_LIMIT) {
			const PhysicalObjectCopy2D nextA = copyPhysicalObject(scene, A, t0);
			const PhysicalObjectCopy2D nextB = copyPhysicalObject(scene, B, t0);

			const GJK2DResult result = GJK2D(nextA, nextB);

			if (gjkResult != nullptr) {
				*gjkResult = result;
			}

			distanceIteration += 1;

			// Exit if the shapes are initially overlapped.
			if (result.distance <= 0.0f) {
				output.state = CCDOutputState::OVERLAPPED;
				output.fraction = 0.0f;

				return output;
			}

			// If the minimum distance is less than the tolerance, exit.
			if (result.distance <= TARGET + TOLERANCE) {
				output.state = CCDOutputState::HIT;
				output.fraction = t0;

				return output;
			}

			const SeparationFunction separationFunction = createSeparationFunction(nextA, nextB, result);

			float32 t1 = 1.0f;
			uint32 pushBackIteration = 0;

			while (pushBackIteration < PUSH_BACK_ITERATION_LIMIT) {
				const PhysicalObjectCopy2D nextSeparationA = copyPhysicalObject(scene, A, t1);
				const PhysicalObjectCopy2D nextSeparationB = copyPhysicalObject(scene, B, t1);
				const MinSeparationOutput minSeparationOutput = findMinSeparation(separationFunction, nextSeparationA, nextSeparationB);

				float32 s1 = minSeparationOutput.separationValue;

				if (s1 > TARGET + TOLERANCE) {
					// If the objects are separated at t1, exit.
					output.state = CCDOutputState::SEPARATED;
					output.fraction = t1;

					return output;
				}

				if (s1 > TARGET - TOLERANCE) {
					// Advance t0.
					t0 = t1;

					break;
				}

				// Calculate the initial separation value of the witness points.
				float32 s0 = evaluateSeparation(separationFunction, scene, A, B, minSeparationOutput, t0);

				if (s0 < TARGET - TOLERANCE) {
					// If the root finder reaches the iteration limit, exit.
					output.state = CCDOutputState::UNRESOLVED_ROOT;
					output.fraction = t0;

					BREAKPOINT();

					return output;
				}

				if (s0 <= TARGET + TOLERANCE) {
					// If the objects hit each other, exit.
					output.state = CCDOutputState::HIT;
					output.fraction = t0;
				}

				// Compute root.
				float32 a0 = t0;
				float32 a1 = t1;

				uint32 rootFinderIteration = 0;

				while (rootFinderIteration < ROOT_FINDER_ITERATION_LIMIT) {
					float32 t;

					if (rootFinderIteration & 1) {
						// Use false position.
						t = a0 + (TARGET - s0) * (a1 - a0) / (s1 - s0);
					}
					else {
						// Use bisection.
						t = (a0 + a1) * 0.5f;
					}

					rootFinderIteration += 1;

					const float32 s = evaluateSeparation(separationFunction, scene, A, B, minSeparationOutput, t);

					if (abs(s - TARGET) < TOLERANCE) {
						// Move t1 back.
						t1 = t;

						break;
					}

					if (s > TARGET) {
						a0 = t;
						s0 = s;
					}
					else {
						a1 = t;
						s1 = s;
					}
				}

				pushBackIteration += 1;
			}
		}

		output.state = CCDOutputState::UNRESOLVED_ROOT;
		output.fraction = t0;

		BREAKPOINT();

		return output;
	}

	SeparationFunction Integrator::createSeparationFunction(const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, const GJK2DResult& gjkResult) {
		SeparationFunction separationFunction;

		if (gjkResult.closestFeatureA.count == 1 && gjkResult.closestFeatureB.count == 1) {
			// Both features are vertices.

			separationFunction.separationType = SeparationType::VERTEX_A | SeparationType::VERTEX_B;

			const vec2<float32>& a = multiplyTransform(A.geometry->vertices[gjkResult.closestFeatureA.vertexIndex], A.transform);
			const vec2<float32>& b = multiplyTransform(B.geometry->vertices[gjkResult.closestFeatureB.vertexIndex], B.transform);

			separationFunction.separatingAxis = normalized(b - a);
			separationFunction.localPoint = {0.0f, 0.0f};

			return separationFunction;
		}

		// At least one feature is an edge. Use it as the separating axis.

		const PhysicalObjectCopy2D* vertexEntity = nullptr;
		const PhysicalObjectCopy2D* edgeEntity = nullptr;
		const ClosestFeature2D* vertexFeature = nullptr;
		const ClosestFeature2D* edgeFeature = nullptr;

		identifyClosestFeatures(
			separationFunction.separationType,
			A,
			B,
			gjkResult.closestFeatureA,
			gjkResult.closestFeatureB,
			vertexEntity,
			edgeEntity,
			vertexFeature,
			edgeFeature
		);

		const vec2<float32>& edgeLocalPoint0 = edgeEntity->geometry->vertices[edgeFeature->edgeIndices[0]];
		const vec2<float32>& edgeLocalPoint1 = edgeEntity->geometry->vertices[edgeFeature->edgeIndices[1]];

		separationFunction.separatingAxis = perpendicular(edgeLocalPoint1 - edgeLocalPoint0, 1.0f);
		separationFunction.localPoint = lerp(edgeLocalPoint0, edgeLocalPoint1, 0.5f);

		const vec2<float32> edgePoint = multiplyTransform(separationFunction.localPoint, edgeEntity->transform);
		const vec2<float32> vertexPoint = multiplyTransform(vertexEntity->geometry->vertices[vertexFeature->vertexIndex], vertexEntity->transform);

		const mat3x3<float32> edgeRotation = mat3x3<float32>::rotation(edgeEntity->rotation);
		const vec2<float32> normal = multiplyTransform(separationFunction.separatingAxis, edgeRotation);

		if (dot(vertexPoint - edgePoint, normal) < 0.0f) {
			// If the direction formed by vertexPoint - edgePoint is facing away from the normal,
			// invert the separating axis.
			separationFunction.separatingAxis = -separationFunction.separatingAxis;
		}

		return separationFunction;
	}

	MinSeparationOutput Integrator::findMinSeparation(SeparationFunction separationFunction, const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B) {
		MinSeparationOutput output{};
		vec2<float32> normal;
		vec2<float32> a;
		vec2<float32> b;

		switch (separationFunction.separationType) {
			case (SeparationType::VERTEX_A | SeparationType::VERTEX_B): {
				normal = separationFunction.separatingAxis;

				// NOTE: The support with canceled rotation doesn't seem to be necessary here.
				a = multiplyTransform(A.geometry->vertices[0], A.transform);
				b = multiplyTransform(B.geometry->vertices[0], B.transform);

				break;
			}
			default: {
				const PhysicalObjectCopy2D* vertexObject = nullptr;
				const PhysicalObjectCopy2D* edgeObject = nullptr;

				identifyClosestFeatures(A, B, separationFunction.separationType, vertexObject, edgeObject);

				const mat3x3<float32> edgeRotation = mat3x3<float32>::rotation(edgeObject->rotation);

				normal = multiplyTransform(separationFunction.separatingAxis, edgeRotation);

				const uint32 i = supportIndex(*vertexObject, -normal);

				output.iA = i;
				output.iB = i;

				a = multiplyTransform(separationFunction.localPoint, edgeObject->transform);
				b = multiplyTransform(vertexObject->geometry->vertices[i], vertexObject->transform);

				break;
			}
		}

		output.separationValue = dot(b - a, normal);

		return output;
	}

	float32 Integrator::evaluateSeparation(SeparationFunction separationFunction, const Scene2D& scene, Entity A, Entity B, MinSeparationOutput minSeparationOutput, float32 t) {
		const ref<Geometry2D>& geometryA = scene.getGeometry(A);
		const ref<Geometry2D>& geometryB = scene.getGeometry(B);
		const PhysicalObjectCopy2D tA = copyPhysicalObject(scene, A, t);
		const PhysicalObjectCopy2D tB = copyPhysicalObject(scene, B, t);

		vec2<float32> a;
		vec2<float32> b;
		vec2<float32> d;

		switch (separationFunction.separationType) {
			case (SeparationType::VERTEX_A | SeparationType::VERTEX_B): {
				a = multiplyTransform(geometryA->vertices[minSeparationOutput.iA], tA.transform);
				b = multiplyTransform(geometryB->vertices[minSeparationOutput.iB], tB.transform);
				d = separationFunction.separatingAxis;

				break;
			}
			case (SeparationType::EDGE_A | SeparationType::VERTEX_B): {
				const mat3x3<float32> rotationA = mat3x3<float32>::rotation(tA.rotation);

				a = multiplyTransform(separationFunction.localPoint, tA.transform);
				b = multiplyTransform(geometryB->vertices[minSeparationOutput.iB], tB.transform);
				d = multiplyTransform(separationFunction.separatingAxis, rotationA);

				break;
			}
			case (SeparationType::VERTEX_A | SeparationType::EDGE_B): {
				const mat3x3<float32> rotationB = mat3x3<float32>::rotation(tB.rotation);

				a = multiplyTransform(separationFunction.localPoint, tB.transform);
				b = multiplyTransform(geometryA->vertices[minSeparationOutput.iA], tA.transform);
				d = multiplyTransform(separationFunction.separatingAxis, rotationB);

				break;
			}
		}

		return dot(b - a, d);
	}

	// TODO: Refactor.
	void Integrator::identifyClosestFeatures(
		SeparationType& separationType,
		const PhysicalObjectCopy2D& objectA,
		const PhysicalObjectCopy2D& objectB,
		const ClosestFeature2D& closestFeatureA,
		const ClosestFeature2D& closestFeatureB,
		const PhysicalObjectCopy2D*& vertexObject,
		const PhysicalObjectCopy2D*& edgeObject,
		const ClosestFeature2D*& vertexFeature,
		const ClosestFeature2D*& edgeFeature
	) {
		if (closestFeatureA.count == 2) {
			separationType = SeparationType::EDGE_A | SeparationType::VERTEX_B;
			vertexObject = &objectB;
			vertexFeature = &closestFeatureB;
			edgeObject = &objectA;
			edgeFeature = &closestFeatureA;

			return;
		}

		if (closestFeatureB.count == 2) {
			separationType = SeparationType::VERTEX_A | SeparationType::EDGE_B;
			vertexObject = &objectA;
			vertexFeature = &closestFeatureA;
			edgeObject = &objectB;
			edgeFeature = &closestFeatureB;

			return;
		}

		ENGINE_ASSERT(false, "Invalid separation type.");
	}

	// TODO: Refactor.
	void Integrator::identifyClosestFeatures(const PhysicalObjectCopy2D& objectA, const PhysicalObjectCopy2D& objectB, SeparationType separationType, const PhysicalObjectCopy2D*& vertexObject, const PhysicalObjectCopy2D*& edgeObject) {
		if (separationType & SeparationType::EDGE_A) {
			vertexObject = &objectB;
			edgeObject = &objectA;

			return;
		}

		if (separationType & SeparationType::EDGE_B) {
			vertexObject = &objectA;
			edgeObject = &objectB;

			return;
		}

		ENGINE_ASSERT(false, "Invalid separation type.");
	}

	void Integrator::advanceSimulation(Scene2D& scene, Entity entity, float32 t) {
		ref<RigidBody2D>& rigidBody = std::static_pointer_cast<RigidBody2D>(scene.getBody(entity));

		rigidBody->position += rigidBody->linearVelocity * t + rigidBody->linearAcceleration * t * t * 0.5f;
		rigidBody->rotation += rigidBody->angularVelocity * t + rigidBody->angularAcceleration * t * t * 0.5f;

		rigidBody->linearVelocity += rigidBody->linearAcceleration * t;
		rigidBody->angularVelocity += rigidBody->angularAcceleration * t;

		scene.updateTransform(entity);
	}
}