#include "pch.hpp"

#include "Engine/API/Physics/GJK2D.hpp"
#include "Engine/API/Physics/PhysicsEngine.hpp"

namespace ProjectA {
	static constexpr uint32 DISTANCE_ITERATION_LIMIT = 20;
	static constexpr uint32 PUSH_BACK_ITERATION_LIMIT = 8;
	static constexpr uint32 ROOT_FINDER_ITERATION_LIMIT = 50;

	// Toggle continuous collision detection.
	#define DEBUG_ENABLE_CCD

	void PhysicsEngine::update(Scene2D& scene, GJK2DResult* gjkResult) {
		this->scene = &scene;

		broadPhase();
		narrowPhase(gjkResult);
		integrate();
	}

	void PhysicsEngine::broadPhase() {
		pairs.clear();
		safeAdvanceTimes.resize(scene->getPhysicalObjectEntityCount());
		impulses.clear();

		std::fill(safeAdvanceTimes.begin(), safeAdvanceTimes.end(), 1.0f);

		if (scene->getPhysicalObjectEntityCount() < 2) {
			return;
		}

		for (const std::vector<Entity>& layer : scene->getLayers()) {
			if (layer.size() < 2) {
				continue;
			}

			for (Entity A = 0; A < layer.size() - 1; A += 1) {
				for (Entity B = A + 1; B < scene->getPhysicalObjectEntityCount(); B += 1) {
					CollisionPair pair{};
					pair.A = A;
					pair.B = B;

					pairs.emplace_back(pair);
				}
			}
		}
	}

	void PhysicsEngine::narrowPhase(GJK2DResult* gjkResult) {
		// TODO
		// BREAKPOINT();

		for (CollisionPair& pair : pairs) {
			float32 toi = 1.0f;

			#ifdef DEBUG_ENABLE_CCD
				const CCDOutput output = evaluateContinuousCollision(pair.A, pair.B, gjkResult);

				toi = output.fraction;
			#endif

			// TODO: lyah::min
			safeAdvanceTimes[pair.A] = std::min(safeAdvanceTimes[pair.A], toi);
			safeAdvanceTimes[pair.B] = std::min(safeAdvanceTimes[pair.B], toi);

			// https://code.tutsplus.com/how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331t
			if (output.state == CCDOutputState::HIT) {
				const PhysicalObjectCopy2D copyA = copyPhysicalObject(*scene, pair.A, toi);
				const PhysicalObjectCopy2D copyB = copyPhysicalObject(*scene, pair.B, toi);

				ref<RigidBody2D>& rigidBodyA = std::static_pointer_cast<RigidBody2D>(scene->getBody(pair.A));
				ref<RigidBody2D>& rigidBodyB = std::static_pointer_cast<RigidBody2D>(scene->getBody(pair.B));
				const ref<Material>& materialA = scene->getMaterial(pair.A);
				const ref<Material>& materialB = scene->getMaterial(pair.B);

				const vec2<float32> n = normalized(copyB.position - copyA.position);
				vec2<float32> relativeLinearVelocity = copyB.linearVelocity - copyA.linearVelocity;
				const float32 velocityAlongNormal = dot(relativeLinearVelocity, n);

				if (velocityAlongNormal > 0.0f) {
					// These entities are separating; continue to the next entities.
					continue;
				}

				// TODO: lyah::min
				// Calculate coefficient of restitution.
				const float32 e = std::min(materialA->coefficientOfRestitution, materialB->coefficientOfRestitution);

				// Calculate linear impulse.
				// Fn is the magnitude of the collision normal.
				const float32 Fn = (-(1 + e) * velocityAlongNormal) / (rigidBodyA->inverseMass + rigidBodyB->inverseMass);
				const vec2<float32> normalImpulse = n * Fn;

				// TODO: Fix infinite tangent vector.
				/* // Linear velocity + normal impulse.
				const vec2<float32> vA = copyA.linearVelocity - normalImpulse * rigidBodyA->inverseMass;
				const vec2<float32> vB = copyB.linearVelocity + normalImpulse * rigidBodyB->inverseMass;

				relativeLinearVelocity = vB - vA;

				// Tangent to the collision normal.
				const vec2<float32> tangent = normalized(relativeLinearVelocity - n * dot(relativeLinearVelocity, n));

				// Magnitude of the friction.
				const float32 Ff = -dot(relativeLinearVelocity, tangent) / (rigidBodyA->inverseMass + rigidBodyB->inverseMass);

				const float32 mu = (rigidBodyA->staticFriction + rigidBodyB->staticFriction) * 0.5f;

				vec2<float32> frictionImpulse = tangent * Ff;

				if (abs(Ff) > mu * Fn) {
					frictionImpulse = tangent * -Fn * ((rigidBodyA->dynamicFriction + rigidBodyB->dynamicFriction) * 0.5f);
				} */

				ImpulseData impulseDataA;
				impulseDataA.entity = pair.A;
				impulseDataA.impulse = -(normalImpulse /* + frictionImpulse */) * rigidBodyA->inverseMass;

				ImpulseData impulseDataB;
				impulseDataB.entity = pair.B;
				impulseDataB.impulse = (normalImpulse /* + frictionImpulse */) * rigidBodyB->inverseMass;

				impulses.emplace_back(impulseDataA);
				impulses.emplace_back(impulseDataB);
			}
		}
	}

	void PhysicsEngine::integrate() {
		for (Entity entity = 0; entity < scene->getPhysicalObjectEntityCount(); entity += 1) {
			const float32 t = safeAdvanceTimes[entity];

			ref<RigidBody2D>& rigidBody = std::static_pointer_cast<RigidBody2D>(scene->getBody(entity));

			rigidBody->position += rigidBody->linearVelocity * t + rigidBody->linearAcceleration * t * t * 0.5f;
			rigidBody->rotation += rigidBody->angularVelocity * t + rigidBody->angularAcceleration * t * t * 0.5f;

			rigidBody->linearVelocity += (rigidBody->linearAcceleration + rigidBody->linearForces) * t * 0.5f;
			rigidBody->angularVelocity += (rigidBody->angularAcceleration + rigidBody->angularForces) * t * 0.5f;

			rigidBody->linearAcceleration = rigidBody->linearForces;
			rigidBody->angularAcceleration = rigidBody->angularForces;

			scene->updateTransform(entity);
		}

		for (const ImpulseData& impulseData : impulses) {
			const float32 toi = safeAdvanceTimes[impulseData.entity];

			// TODO: lyah::max
			// TODO: max(0.1, t) seems to be needed with 2 entities. Both don't work when gravity is enabled.
			const float32 t = 1.0f - toi;
			// const float32 t = std::max(0.1f, 1.0f - toi);

			ref<RigidBody2D>& rigidBody = std::static_pointer_cast<RigidBody2D>(scene->getBody(impulseData.entity));

			rigidBody->linearVelocity += impulseData.impulse;

			rigidBody->position += rigidBody->linearVelocity * t + rigidBody->linearAcceleration * t * t * 0.5f;
			rigidBody->rotation += rigidBody->angularVelocity * t + rigidBody->angularAcceleration * t * t * 0.5f;

			rigidBody->linearVelocity += (rigidBody->linearAcceleration + rigidBody->linearForces) * t * 0.5f;
			rigidBody->angularVelocity += (rigidBody->angularAcceleration + rigidBody->angularForces) * t * 0.5f;

			rigidBody->linearAcceleration = rigidBody->linearForces;
			rigidBody->angularAcceleration = rigidBody->angularForces;

			scene->updateTransform(impulseData.entity);
		}
	}

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
	CCDOutput PhysicsEngine::evaluateContinuousCollision(Entity A, Entity B, GJK2DResult* gjkResult) {
		static constexpr float32 TOLERANCE = 0.25f * 0.005f;
		static constexpr float32 TARGET = 0.005f;

		ENGINE_STATIC_ASSERT(TARGET > TOLERANCE);

		CCDOutput output{};

		output.state = CCDOutputState::UNDETERMINED;
		output.fraction = 0.0f;

		float32 t0 = 0.0f;

		const PhysicalObjectCopy2D baseA = copyPhysicalObject(*scene, A);
		const PhysicalObjectCopy2D baseB = copyPhysicalObject(*scene, B);

		uint32 distanceIteration = 0;

		while (distanceIteration < DISTANCE_ITERATION_LIMIT) {
			const PhysicalObjectCopy2D nextA = copyPhysicalObject(*scene, A, t0);
			const PhysicalObjectCopy2D nextB = copyPhysicalObject(*scene, B, t0);

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
				const PhysicalObjectCopy2D nextSeparationA = copyPhysicalObject(*scene, A, t1);
				const PhysicalObjectCopy2D nextSeparationB = copyPhysicalObject(*scene, B, t1);
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
				float32 s0 = evaluateSeparation(separationFunction, A, B, minSeparationOutput, t0);

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

					const float32 s = evaluateSeparation(separationFunction, A, B, minSeparationOutput, t);

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

	SeparationFunction PhysicsEngine::createSeparationFunction(const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, const GJK2DResult& gjkResult) {
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

	MinSeparationOutput PhysicsEngine::findMinSeparation(SeparationFunction separationFunction, const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B) {
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

	float32 PhysicsEngine::evaluateSeparation(SeparationFunction separationFunction, Entity A, Entity B, MinSeparationOutput minSeparationOutput, float32 t) {
		const ref<Geometry2D>& geometryA = scene->getGeometry(A);
		const ref<Geometry2D>& geometryB = scene->getGeometry(B);
		const PhysicalObjectCopy2D tA = copyPhysicalObject(*scene, A, t);
		const PhysicalObjectCopy2D tB = copyPhysicalObject(*scene, B, t);

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
	void PhysicsEngine::identifyClosestFeatures(
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
	void PhysicsEngine::identifyClosestFeatures(const PhysicalObjectCopy2D& objectA, const PhysicalObjectCopy2D& objectB, SeparationType separationType, const PhysicalObjectCopy2D*& vertexObject, const PhysicalObjectCopy2D*& edgeObject) {
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

	void PhysicsEngine::advanceSimulation(Entity entity, float32 t) {
		ref<RigidBody2D>& rigidBody = std::static_pointer_cast<RigidBody2D>(scene->getBody(entity));

		rigidBody->position += rigidBody->linearVelocity * t + rigidBody->linearAcceleration * t * t * 0.5f;
		rigidBody->rotation += rigidBody->angularVelocity * t + rigidBody->angularAcceleration * t * t * 0.5f;

		rigidBody->linearVelocity += (rigidBody->linearAcceleration + rigidBody->linearForces) * t * 0.5f;
		rigidBody->angularVelocity += (rigidBody->angularAcceleration + rigidBody->angularForces) * t * 0.5f;

		rigidBody->linearAcceleration = rigidBody->linearForces;
		rigidBody->angularAcceleration = rigidBody->angularForces;

		scene->updateTransform(entity);
	}
}