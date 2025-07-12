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
			if (layer.empty()) {
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
		for (CollisionPair& pair : pairs) {
			CCDOutput output;

			#ifdef DEBUG_ENABLE_CCD
				output = evaluateContinuousCollision(pair.A, pair.B, gjkResult);
			#else
				output.state = CCDOutputState::SEPARATED;
				output.fraction = 1.0f;
			#endif

			const float32 toi = output.fraction;

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

				// Vector from the center of mass to the contact point on A.
				const vec2<float32> rA = output.closestA - copyA.geometry->centerOfMass;

				// Vector from the center of mass to the contact point on B.
				const vec2<float32> rB = output.closestB - copyB.geometry->centerOfMass;

				ImpulseData impulseDataA;
				impulseDataA.entity = pair.A;
				impulseDataA.impulse = -normalImpulse /* - frictionImpulse */;
				impulseDataA.r = rA;

				ImpulseData impulseDataB;
				impulseDataB.entity = pair.B;
				impulseDataB.impulse = +normalImpulse /* + frictionImpulse */;
				impulseDataB.r = rB;

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
			// const float32 t = 1.0f - toi;
			const float32 t = std::max(0.1f, 1.0f - toi);

			ref<RigidBody2D>& rigidBody = std::static_pointer_cast<RigidBody2D>(scene->getBody(impulseData.entity));

			rigidBody->linearVelocity += impulseData.impulse * rigidBody->inverseMass;
			rigidBody->angularVelocity += area(impulseData.r, impulseData.impulse) * rigidBody->inverseInertia;

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
	// Evaluates continuous collisions between A and B.
	// The result of the last GJK call is copied to `gjkResult` if it's not null.
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

			output.closestA = result.closestA;
			output.closestB = result.closestB;

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
				output.closestB = result.closestB;

				return output;
			}

			const SeparationFunction separationFunction(nextA, nextB, result.closestFeatureA, result.closestFeatureB);

			float32 t1 = 1.0f;
			uint32 pushBackIteration = 0;

			while (pushBackIteration < PUSH_BACK_ITERATION_LIMIT) {
				const PhysicalObjectCopy2D nextSeparationA = copyPhysicalObject(*scene, A, t1);
				const PhysicalObjectCopy2D nextSeparationB = copyPhysicalObject(*scene, B, t1);
				const MinSeparationOutput minSeparationOutput = separationFunction.findMinimumValue(nextSeparationA, nextSeparationB);

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

				const PhysicalObjectCopy2D tA = copyPhysicalObject(*scene, A, t0);
				const PhysicalObjectCopy2D tB = copyPhysicalObject(*scene, B, t0);

				// Calculate the initial separation value of the witness points.
				float32 s0 = separationFunction.evaluate(minSeparationOutput, tA, tB);

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

					const PhysicalObjectCopy2D tA = copyPhysicalObject(*scene, A, t);
					const PhysicalObjectCopy2D tB = copyPhysicalObject(*scene, B, t);

					const float32 s = separationFunction.evaluate(minSeparationOutput, tA, tB);

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