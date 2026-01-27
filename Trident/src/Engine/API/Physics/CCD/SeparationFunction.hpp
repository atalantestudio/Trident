// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/CCD/MinSeparationOutput.hpp"
#include "Engine/API/Physics/Feature2D.hpp"
#include "Engine/API/Physics/support.hpp"
#include "Engine/API/Scene/Scene2D.hpp"

namespace Trident {
	/* struct PhysicalObjectCopy2D {
		Entity entity;

		mat3x3<float32> transform;

		vec2<float32> position;
		vec2<float32> linearVelocity;
		vec2<float32> force;

		float32 rotation;
		float32 angularVelocity;
		float32 torque;

		vec2<float32> scale;

		ref<Geometry2D> geometry;
	};

	static PhysicalObjectCopy2D copyPhysicalObject(const Scene2D& scene, Entity entity) {
		const RigidBody2D& rigidBody = scene.getRigidBody(entity);
		const ref<Geometry2D> geometry = scene.getGeometry(entity);

		PhysicalObjectCopy2D copy{};
		copy.entity = entity;
		copy.transform = rigidBody.transform;
		copy.position = rigidBody.position;
		copy.linearVelocity = rigidBody.linearVelocity;
		copy.force = rigidBody.force;
		copy.rotation = rigidBody.rotation;
		copy.angularVelocity = rigidBody.angularVelocity;
		copy.torque = rigidBody.torque;
		copy.scale = rigidBody.scale;
		copy.geometry = geometry;

		return copy;
	}

	static PhysicalObjectCopy2D copyPhysicalObject(const Scene2D& scene, Entity entity, float32 t) {
		PhysicalObjectCopy2D copy = copyPhysicalObject(scene, entity);

		copy.position += copy.linearVelocity * t + copy.force * t * t * 0.5f;
		copy.rotation += copy.angularVelocity * t + copy.torque * t * t * 0.5f;

		copy.linearVelocity += copy.linearAcceleration * t;
		copy.angularVelocity += copy.torque * t;

		copy.transform =
			mat3x3<float32>::translation(-copy.geometry->centerOfMass) *
			mat3x3<float32>::scaling(copy.scale) *
			mat3x3<float32>::rotation(-copy.rotation) *
			mat3x3<float32>::translation(copy.position);

		return copy;
	}

	class SeparationFunction;

	typedef MinSeparationOutput (*FindMinSeparationFunction)(const SeparationFunction& separationFunction, const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, uint32 edgeObjectIndex, uint32 vertexObjectIndex);
	typedef float32 (*EvaluateSeparationFunction)(const SeparationFunction& separationFunction, const MinSeparationOutput& minSeparationOutput, const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, uint32 edgeObjectIndex, uint32 vertexObjectIndex);

	static MinSeparationOutput findMinVertexVertexSeparation(const SeparationFunction& separationFunction, const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, uint32 edgeObjectIndex, uint32 vertexObjectIndex);
	static MinSeparationOutput findMinEdgeVertexSeparation(const SeparationFunction& separationFunction, const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, uint32 edgeObjectIndex, uint32 vertexObjectIndex);
	static float32 evaluateVertexVertexSeparation(const SeparationFunction& separationFunction, const MinSeparationOutput& minSeparationOutput, const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, uint32 edgeObjectIndex, uint32 vertexObjectIndex);
	static float32 evaluateEdgeVertexSeparation(const SeparationFunction& separationFunction, const MinSeparationOutput& minSeparationOutput, const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, uint32 edgeObjectIndex, uint32 vertexObjectIndex);

	class SeparationFunction {
		public:
			explicit SeparationFunction(const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, const Feature2D& closestFeatureA, const Feature2D& closestFeatureB) {
				if (closestFeatureA.getVertexCount() == 1 && closestFeatureB.getVertexCount() == 1) {
					const vec2<float32> a = closestFeatureA.vertex;
					const vec2<float32> b = closestFeatureB.vertex;

					// TODO: Does it need to be normalized?
					separatingAxis = normalized(b - a);

					localPoint = {0.0f, 0.0f};

					_findMinSeparation = findMinVertexVertexSeparation;
					_evaluateSeparation = evaluateVertexVertexSeparation;

					return;
				}

				const Feature2D* vertexFeature = nullptr;
				const Feature2D* edgeFeature = nullptr;

				if (closestFeatureA.getVertexCount() == 2) {
					_vertexObjectIndex = 1;
					vertexFeature = &closestFeatureB;
					_edgeObjectIndex = 0;
					edgeFeature = &closestFeatureA;
				}
				else if (closestFeatureB.getVertexCount() == 2) {
					_vertexObjectIndex = 0;
					vertexFeature = &closestFeatureA;
					_edgeObjectIndex = 1;
					edgeFeature = &closestFeatureB;
				}
				else {
					ENGINE_ASSERT(false, "Invalid separation type.");
				}

				const PhysicalObjectCopy2D* objects[2] = {&A, &B};
				const PhysicalObjectCopy2D& edgeObject = *objects[_edgeObjectIndex];
				const PhysicalObjectCopy2D& vertexObject = *objects[_vertexObjectIndex];

				const vec2<float32> edgeLocalPoint0 = edgeObject.geometry->vertices[edgeFeature->edgeIndices[0]];
				const vec2<float32> edgeLocalPoint1 = edgeObject.geometry->vertices[edgeFeature->edgeIndices[1]];

				separatingAxis = perpendicularRight(edgeLocalPoint1 - edgeLocalPoint0);
				localPoint = lerp(edgeLocalPoint0, edgeLocalPoint1, 0.5f);

				const vec2<float32> edgePoint = multiplyTransform(localPoint, edgeObject.transform);
				const vec2<float32> vertexPoint = multiplyTransform(vertexObject.geometry->vertices[vertexFeature->vertexIndex], vertexObject.transform);

				const mat3x3<float32> edgeRotation = mat3x3<float32>::rotation(edgeObject.rotation);
				const vec2<float32> normal = multiplyTransform(separatingAxis, edgeRotation);

				if (dot(vertexPoint - edgePoint, normal) < 0.0f) {
					// If vertexPoint - edgePoint is facing away from the normal, invert the separating axis.
					separatingAxis = -separatingAxis;
				}

				_findMinSeparation = findMinEdgeVertexSeparation;
				_evaluateSeparation = evaluateEdgeVertexSeparation;
			}

			MinSeparationOutput findMinimumValue(const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B) const {
				return _findMinSeparation(*this, A, B, _edgeObjectIndex, _vertexObjectIndex);
			}

			float32 evaluate(const MinSeparationOutput& minSeparationOutput, const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B) const {
				return _evaluateSeparation(*this, minSeparationOutput, A, B, _edgeObjectIndex, _vertexObjectIndex);
			}

		public:
			// The separating axis is only used as a direction to find support points,
			// and does not need to be normalized.
			vec2<float32> separatingAxis;

			// The average of the separation points.
			vec2<float32> localPoint;

		private:
			// NOTE: Not used with vertex-vertex separations.
			uint32 _edgeObjectIndex;
			uint32 _vertexObjectIndex;

			FindMinSeparationFunction _findMinSeparation;
			EvaluateSeparationFunction _evaluateSeparation;
	};

	MinSeparationOutput findMinVertexVertexSeparation(const SeparationFunction& separationFunction, const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, uint32 edgeObjectIndex, uint32 vertexObjectIndex) {
		MinSeparationOutput output;

		const vec2<float32> axis = separationFunction.separatingAxis;

		// TODO: The support with canceled rotation doesn't seem to be necessary here.
		const vec2<float32> a = multiplyTransform(A.geometry->vertices[0], A.transform);
		const vec2<float32> b = multiplyTransform(B.geometry->vertices[0], B.transform);

		output.separationValue = dot(b - a, axis);

		return output;
	}

	MinSeparationOutput findMinEdgeVertexSeparation(const SeparationFunction& separationFunction, const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, uint32 edgeObjectIndex, uint32 vertexObjectIndex) {
		MinSeparationOutput output{};

		const PhysicalObjectCopy2D* objects[2] = {&A, &B};
		const PhysicalObjectCopy2D& edgeObject = *objects[edgeObjectIndex];
		const PhysicalObjectCopy2D& vertexObject = *objects[vertexObjectIndex];
		const vec2<float32> axis = multiplyTransform(separationFunction.separatingAxis, mat3x3<float32>::rotation(edgeObject.rotation));
		const uint32 i = supportIndex(vertexObject, -axis);

		output.iA = i;
		output.iB = i;

		const vec2<float32> a = multiplyTransform(separationFunction.localPoint, edgeObject.transform);
		const vec2<float32> b = multiplyTransform(vertexObject.geometry->vertices[i], vertexObject.transform);

		output.separationValue = dot(b - a, axis);

		return output;
	}

	float32 evaluateVertexVertexSeparation(const SeparationFunction& separationFunction, const MinSeparationOutput& minSeparationOutput, const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, uint32 edgeObjectIndex, uint32 vertexObjectIndex) {
		// TODO: Don't select an arbitraty vertex from the geometry.
		const vec2<float32> a = multiplyTransform(A.geometry->vertices[0], A.transform);
		const vec2<float32> b = multiplyTransform(B.geometry->vertices[0], B.transform);

		return dot(b - a, separationFunction.separatingAxis);
	}

	float32 evaluateEdgeVertexSeparation(const SeparationFunction& separationFunction, const MinSeparationOutput& minSeparationOutput, const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, uint32 edgeObjectIndex, uint32 vertexObjectIndex) {
		const PhysicalObjectCopy2D* objects[2] = {&A, &B};
		const PhysicalObjectCopy2D& edgeObject = *objects[edgeObjectIndex];
		const PhysicalObjectCopy2D& vertexObject = *objects[vertexObjectIndex];
		const uint32 vertexObjectVertexIndex = *(&minSeparationOutput.iA + vertexObjectIndex);
		const vec2<float32> a = multiplyTransform(separationFunction.localPoint, edgeObject.transform);
		const vec2<float32> b = multiplyTransform(vertexObject.geometry->vertices[vertexObjectVertexIndex], vertexObject.transform);
		const vec2<float32> separatingAxis = multiplyTransform(separationFunction.separatingAxis, mat3x3<float32>::rotation(edgeObject.rotation));

		return dot(b - a, separatingAxis);
	} */
}