// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Scene/StaticSceneBuilder.hpp"

namespace Trident {
	StaticSceneBuilder::StaticSceneBuilder() {}

	view<svec3<float32>> StaticSceneBuilder::getVertexBuffer() const {
		return view<svec3<float32>>(&vertexBuffer[0], vertexBuffer.size());
	}

	view<uint16> StaticSceneBuilder::getIndexBuffer() const {
		return view<uint16>(&indexBuffer[0], indexBuffer.size());
	}

	view<StaticObject> StaticSceneBuilder::getObjects() const {
		return view<StaticObject>(&objects[0], objects.size());
	}

	void StaticSceneBuilder::createStaticObject(const StaticObjectDescriptor& descriptor) {
		StaticObject object{};
		object.vertexPositionIndices = sequence<uint16>(6);

		uint16 objectVertexIndex = 0;

		while (objectVertexIndex < descriptor.vertices.count()) {
			const svec3<float32> vertex = descriptor.vertices[objectVertexIndex];

			const uint16 index = findVertex(vertex);

			if (index == vertexBuffer.size()) {
				vertexBuffer.emplace_back(vertex);
			}

			object.vertexPositionIndices[objectVertexIndex] = index;

			objectVertexIndex += 1;
		}

		object.texture =
			mat3x3<float32>::scaling(descriptor.textureFiltering.scale) *
			mat3x3<float32>::rotation(descriptor.textureFiltering.rotation) *
			mat3x3<float32>::translation(descriptor.textureFiltering.translation);

		objects.emplace_back(object);
	}

	uint16 StaticSceneBuilder::findVertex(svec3<float32> vertex) const {
		constexpr float32 componentEpsilon = 0.001f;

		uint16 vertexIndex = 0;

		while (vertexIndex < vertexBuffer.size()) {
			const svec3<float32> other = vertexBuffer[vertexIndex];

			if (
				vertex[0] > other[0] - componentEpsilon && vertex[0] < other[0] + componentEpsilon &&
				vertex[1] > other[1] - componentEpsilon && vertex[1] < other[1] + componentEpsilon &&
				vertex[2] > other[2] - componentEpsilon && vertex[2] < other[2] + componentEpsilon
			) {
				break;
			}

			vertexIndex += 1;
		}

		return vertexIndex;
	}
}