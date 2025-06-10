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
		for (uint32 i = 0; i < scene.getObjectCount() - 1; i += 1) {
			for (uint32 j = i + 1; j < scene.getObjectCount(); j += 1) {
				Object2D& A = scene[i];
				Object2D& B = scene[j];

				float32 toi = 1.0f;

				#ifdef DEBUG_ENABLE_CCD
					const CCDOutput output = evaluateContinuousCollision(A, B, gjkResult);

					#ifdef BUILD_MODE_DEBUG
						printOutputState(output.state);
					#endif

					toi = output.fraction;
				#endif

				advanceSimulation(A, toi);
				advanceSimulation(B, toi);

				// TODO: Resolve possible collisions.
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
	CCDOutput Integrator::evaluateContinuousCollision(const Object2D& A, const Object2D& B, GJK2DResult* gjkResult) {
		static constexpr float32 TOLERANCE = 0.25f * 0.005f;
		static constexpr float32 TARGET = 0.005f;

		DEBUG_STATIC_ASSERT(TARGET > TOLERANCE);

		CCDOutput output{};

		output.state = CCDOutputState::UNDETERMINED;
		output.fraction = 0.0f;

		float32 t0 = 0.0f;

		uint32 distanceIteration = 0;

		// TODO: Add iteration limit.
		while (true) {
			const Object2D nextA = copyObjectAt(A, t0);
			const Object2D nextB = copyObjectAt(B, t0);

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

			// TODO: Add iteration limit.
			while (true) {
				const Object2D nextSeparationA = copyObjectAt(A, t1);
				const Object2D nextSeparationB = copyObjectAt(B, t1);

				const MinSeparationOutput minSeparationOutput = findMinSeparation(separationFunction, nextSeparationA, nextSeparationB);

				float32 s1 = minSeparationOutput.value;

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

				while (true) {
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

					if (rootFinderIteration >= ROOT_FINDER_ITERATION_LIMIT) {
						break;
					}
				}

				pushBackIteration += 1;

				if (pushBackIteration >= PUSH_BACK_ITERATION_LIMIT) {
					break;
				}
			}

			if (distanceIteration >= DISTANCE_ITERATION_LIMIT) {
				// Root finder error.
				break;
			}
		}

		output.state = CCDOutputState::UNRESOLVED_ROOT;
		output.fraction = t0;

		BREAKPOINT();

		return output;
	}

	Object2D Integrator::copyObjectAt(Object2D A, float32 t) {
		advanceSimulation(A, t);

		return A;
	}

	SeparationFunction Integrator::createSeparationFunction(const Object2D& A, const Object2D& B, const GJK2DResult& gjkResult) {
		const ref<Geometry2D>& geometryA = A.getGeometry();
		const ref<Geometry2D>& geometryB = B.getGeometry();

		SeparationFunction separationFunction;

		if (gjkResult.closestFeatureA.count == 1 && gjkResult.closestFeatureB.count == 1) {
			// Both features are vertices.
			separationFunction.separationType = SeparationType::VERTEX_A | SeparationType::VERTEX_B;

			const vec2<float32>& a = multiplyTransform(geometryA->vertices[gjkResult.closestFeatureA.vertexIndex], A.transform);
			const vec2<float32>& b = multiplyTransform(geometryB->vertices[gjkResult.closestFeatureB.vertexIndex], B.transform);

			// The separating axis is b - a.
			separationFunction.separatingAxis = b - a;
			separationFunction.localPoint = {0.0f, 0.0f};

			return separationFunction;
		}

		// At least one feature is an edge. Use it as the separating axis.
		const Object2D* vertexObjectPtr = nullptr;
		const Object2D* edgeObjectPtr = nullptr;
		const ClosestFeature2D* vertexFeaturePtr = nullptr;
		const ClosestFeature2D* edgeFeaturePtr = nullptr;

		identifyClosestFeatures(
			separationFunction.separationType,
			A,
			B,
			gjkResult.closestFeatureA,
			gjkResult.closestFeatureB,
			vertexObjectPtr,
			edgeObjectPtr,
			vertexFeaturePtr,
			edgeFeaturePtr
		);

		DEBUG_ASSERT(vertexObjectPtr != nullptr && edgeObjectPtr != nullptr && vertexFeaturePtr != nullptr && edgeFeaturePtr != nullptr);

		const Object2D& vertexObject = *vertexObjectPtr;
		const Object2D& edgeObject = *edgeObjectPtr;
		const ref<Geometry2D>& vertexGeometry = vertexObject.getGeometry();
		const ref<Geometry2D>& edgeGeometry = edgeObject.getGeometry();
		const ClosestFeature2D& vertexFeature = *vertexFeaturePtr;
		const ClosestFeature2D& edgeFeature = *edgeFeaturePtr;

		const vec2<float32>& edgeLocalPoint0 = edgeGeometry->vertices[edgeFeature.edgeIndices[0]];
		const vec2<float32>& edgeLocalPoint1 = edgeGeometry->vertices[edgeFeature.edgeIndices[1]];

		separationFunction.separatingAxis = perpendicular(edgeLocalPoint1 - edgeLocalPoint0, 1.0f);

		separationFunction.localPoint = lerp(edgeLocalPoint0, edgeLocalPoint1, 0.5f);

		const vec2<float32> edgePoint = multiplyTransform(separationFunction.localPoint, edgeObject.transform);
		const vec2<float32> vertexPoint = multiplyTransform(vertexGeometry->vertices[vertexFeature.vertexIndex], vertexObject.transform);

		const mat3x3<float32> edgeRotation = mat3x3<float32>::rotation(edgeObject.rotation);
		const vec2<float32> normal = multiplyTransform(separationFunction.separatingAxis, edgeRotation);

		if (dot(vertexPoint - edgePoint, normal) < 0.0f) {
			// If the direction formed by vertexPoint - edgePoint is facing away from the normal,
			// invert the separating axis.
			separationFunction.separatingAxis = -separationFunction.separatingAxis;
		}

		return separationFunction;
	}

	MinSeparationOutput Integrator::findMinSeparation(SeparationFunction separationFunction, const Object2D& A, const Object2D& B) {
		MinSeparationOutput output{};

		vec2<float32> normal;
		vec2<float32> a;
		vec2<float32> b;

		switch (separationFunction.separationType) {
			case (SeparationType::VERTEX_A | SeparationType::VERTEX_B): {
				normal = separationFunction.separatingAxis;

				// TODO: Does the object rotation need to be canceled?
				output.iA = supportIndex(A, normal);
				output.iB = supportIndex(B, -normal);

				a = multiplyTransform(A.getGeometry()->vertices[output.iA], A.transform);
				b = multiplyTransform(B.getGeometry()->vertices[output.iB], B.transform);

				break;
			}
			default: {
				const Object2D* vertexObject = nullptr;
				const Object2D* edgeObject = nullptr;

				identifyClosestFeatures(A, B, separationFunction.separationType, vertexObject, edgeObject);

				DEBUG_ASSERT(vertexObject != nullptr && edgeObject != nullptr);

				const mat3x3<float32> edgeRotation = mat3x3<float32>::rotation(edgeObject->rotation);

				normal = multiplyTransform(separationFunction.separatingAxis, edgeRotation);

				// TODO: Does the object rotation need to be canceled?
				const uint32 i = supportIndex(*vertexObject, -normal);

				output.iA = i;
				output.iB = i;

				a = multiplyTransform(separationFunction.localPoint, edgeObject->transform);
				b = multiplyTransform(vertexObject->getGeometry()->vertices[i], vertexObject->transform);

				break;
			}
		}

		output.value = dot(b - a, normal);

		return output;
	}

	float32 Integrator::evaluateSeparation(SeparationFunction separationFunction, const Object2D& A, const Object2D& B, MinSeparationOutput minSeparationOutput, float32 t) {
		const ref<Geometry2D>& geometryA = A.getGeometry();
		const ref<Geometry2D>& geometryB = B.getGeometry();
		const Object2D& tA = copyObjectAt(A, t);
		const Object2D& tB = copyObjectAt(B, t);

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

	void Integrator::advanceSimulation(Object2D& A, float32 t) {
		A.position += A.linearVelocity * t + A.linearAcceleration * t * t * 0.5f;
		A.rotation += A.angularVelocity * t + A.angularAcceleration * t * t * 0.5f;

		A.linearVelocity += A.linearAcceleration * t;
		A.angularVelocity += A.angularAcceleration * t;

		A.updateTransform();
	}

	void Integrator::identifyClosestFeatures(
		SeparationType& separationType,
		const Object2D& objectA,
		const Object2D& objectB,
		const ClosestFeature2D& closestFeatureA,
		const ClosestFeature2D& closestFeatureB,
		const Object2D*& vertexObject,
		const Object2D*& edgeObject,
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

		DEBUG_ASSERT(false);
	}

	void Integrator::identifyClosestFeatures(
		const Object2D& objectA,
		const Object2D& objectB,
		SeparationType separationType,
		const Object2D*& vertexObject,
		const Object2D*& edgeObject
	) {
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

		DEBUG_ASSERT(false);
	}
}