#pragma once

#include "Engine/API/Physics/CCD/SeparationFunction.hpp"
#include "Engine/API/Physics/CCD/TimeOfImpactState.hpp"
#include "Engine/API/Physics/GJK2D.hpp"
#include "Engine/API/Scene/Scene2D.hpp"

namespace ProjectA {
	/* struct TimeOfImpact {
		TimeOfImpactState state;

		// Time in between 2 frames at which point the objects collide.
		// E [0.0, 1.0].
		float32 fraction;

		// Closest points on A and B returned from the last GJK iteration.
		vec2<float32> closestA;
		vec2<float32> closestB;
	};

	static constexpr uint32 DISTANCE_ITERATION_LIMIT = 20;
	static constexpr uint32 PUSH_BACK_ITERATION_LIMIT = 8;
	static constexpr uint32 ROOT_FINDER_ITERATION_LIMIT = 50;

	// Bilateral Advancement algorithm implementation.
	// Evaluates continuous collisions between A and B.
	// The result of the last GJK call is copied to `gjkOutput` if it's not null.
	static TimeOfImpact calculateTimeOfImpact(Scene2D* scene, Entity A, Entity B, GJK2DOutput* gjkOutputPointer) {
		static constexpr float32 TOLERANCE = 0.25f * 0.005f;
		static constexpr float32 TARGET = 0.005f;

		ENGINE_STATIC_ASSERT(TARGET > TOLERANCE);

		TimeOfImpact output{};

		output.state = TimeOfImpactState::UNDETERMINED;
		output.fraction = 0.0f;

		float32 t0 = 0.0f;

		const PhysicalObjectCopy2D baseA = copyPhysicalObject(*scene, A);
		const PhysicalObjectCopy2D baseB = copyPhysicalObject(*scene, B);

		uint32 distanceIteration = 0;

		while (distanceIteration < DISTANCE_ITERATION_LIMIT) {
			const PhysicalObjectCopy2D nextA = copyPhysicalObject(*scene, A, t0);
			const PhysicalObjectCopy2D nextB = copyPhysicalObject(*scene, B, t0);

			const GJK2DOutput gjkOutput = GJK2D(nextA, nextB);

			if (gjkOutputPointer != nullptr) {
				*gjkOutputPointer = gjkOutput;
			}

			distanceIteration += 1;

			output.closestA = gjkOutput.closestA;
			output.closestB = gjkOutput.closestB;

			// Exit if the shapes are initially overlapped.
			if (gjkOutput.distance <= 0.0f) {
				output.state = TimeOfImpactState::OVERLAPPED;
				output.fraction = 0.0f;

				return output;
			}

			// If the minimum distance is less than the tolerance, exit.
			if (gjkOutput.distance <= TARGET + TOLERANCE) {
				output.state = TimeOfImpactState::HIT;
				output.fraction = t0;
				output.closestB = gjkOutput.closestB;

				return output;
			}

			const SeparationFunction separationFunction(nextA, nextB, gjkOutput.closestFeatureA, gjkOutput.closestFeatureB);

			float32 t1 = 1.0f;
			uint32 pushBackIteration = 0;

			while (pushBackIteration < PUSH_BACK_ITERATION_LIMIT) {
				const PhysicalObjectCopy2D nextSeparationA = copyPhysicalObject(*scene, A, t1);
				const PhysicalObjectCopy2D nextSeparationB = copyPhysicalObject(*scene, B, t1);
				const MinSeparationOutput minSeparationOutput = separationFunction.findMinimumValue(nextSeparationA, nextSeparationB);

				float32 s1 = minSeparationOutput.separationValue;

				if (s1 > TARGET + TOLERANCE) {
					// If the objects are separated at t1, exit.
					output.state = TimeOfImpactState::SEPARATED;
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
					output.state = TimeOfImpactState::UNRESOLVED_ROOT;
					output.fraction = t0;

					BREAKPOINT();

					return output;
				}

				if (s0 <= TARGET + TOLERANCE) {
					// If the objects hit each other, exit.
					output.state = TimeOfImpactState::HIT;
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

		output.state = TimeOfImpactState::UNRESOLVED_ROOT;
		output.fraction = t0;

		BREAKPOINT();

		return output;
	} */
}