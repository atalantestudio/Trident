#pragma once

#include "Engine/API/Physics/CCD/MinSeparationOutput.hpp"

namespace ProjectA {
	class SeparationFunction;

	typedef MinSeparationOutput (*FindMinSeparationFunction)(const SeparationFunction& separationFunction, const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, uint32 edgeObjectIndex, uint32 vertexObjectIndex);
	typedef float32 (*EvaluateSeparationFunction)(const SeparationFunction& separationFunction, const MinSeparationOutput& minSeparationOutput, const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, uint32 edgeObjectIndex, uint32 vertexObjectIndex);

	static MinSeparationOutput findMinVertexVertexSeparation(const SeparationFunction& separationFunction, const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, uint32 edgeObjectIndex, uint32 vertexObjectIndex);
	static MinSeparationOutput findMinEdgeVertexSeparation(const SeparationFunction& separationFunction, const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, uint32 edgeObjectIndex, uint32 vertexObjectIndex);
	static float32 evaluateVertexVertexSeparation(const SeparationFunction& separationFunction, const MinSeparationOutput& minSeparationOutput, const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, uint32 edgeObjectIndex, uint32 vertexObjectIndex);
	static float32 evaluateEdgeVertexSeparation(const SeparationFunction& separationFunction, const MinSeparationOutput& minSeparationOutput, const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, uint32 edgeObjectIndex, uint32 vertexObjectIndex);

	class SeparationFunction {
		public:
			explicit SeparationFunction(const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B, const ClosestFeature2D& closestFeatureA, const ClosestFeature2D& closestFeatureB) {
				if (closestFeatureA.count == 1 && closestFeatureB.count == 1) {
					const vec2<float32>& a = closestFeatureA.vertex;
					const vec2<float32>& b = closestFeatureB.vertex;

					// TODO: Does it need to be normalized?
					separatingAxis = normalized(b - a);

					localPoint = {0.0f, 0.0f};

					_findMinSeparation = findMinVertexVertexSeparation;
					_evaluateSeparation = evaluateVertexVertexSeparation;

					return;
				}

				const ClosestFeature2D* vertexFeature = nullptr;
				const ClosestFeature2D* edgeFeature = nullptr;

				if (closestFeatureA.count == 2) {
					_vertexObjectIndex = 1;
					vertexFeature = &closestFeatureB;
					_edgeObjectIndex = 0;
					edgeFeature = &closestFeatureA;
				}
				else if (closestFeatureB.count == 2) {
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

				const vec2<float32>& edgeLocalPoint0 = edgeObject.geometry->vertices[edgeFeature->edgeIndices[0]];
				const vec2<float32>& edgeLocalPoint1 = edgeObject.geometry->vertices[edgeFeature->edgeIndices[1]];

				separatingAxis = perpendicular(edgeLocalPoint1 - edgeLocalPoint0, 1.0f);
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

		const vec2<float32>& axis = separationFunction.separatingAxis;

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
	}
}