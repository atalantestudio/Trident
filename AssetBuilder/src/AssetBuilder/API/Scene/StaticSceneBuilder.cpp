// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Scene/StaticSceneBuilder.hpp"

#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	StaticSceneBuilder::StaticSceneBuilder(std::filesystem::path rootOutputPath) :
		assetLibrary(new AssetLibrary(rootOutputPath))
	{}

	StaticSceneBuilder::~StaticSceneBuilder() {
		delete assetLibrary;
	}

	void StaticSceneBuilder::createStaticObject(const StaticObjectDescriptor& descriptor) {
		StaticObject object{};
		object.geometryAsset = assetLibrary->getAsset<GeometryAsset>(descriptor.geometryAssetName);
		object.transform = descriptor.transform;

		/* uint16 objectVertexIndex = 0;

		while (objectVertexIndex < descriptor.vertices.count()) {
			const svec3<float32> vertex = descriptor.vertices[objectVertexIndex];

			const uint16 index = findVertex(vertex);

			if (index == vertexBuffer.size()) {
				vertexBuffer.emplace_back(vertex);
			}

			object.vertexPositionIndices[objectVertexIndex] = index;

			objectVertexIndex += 1;
		} */

		object.texture =
			mat3x3<float32>::scaling(descriptor.textureFiltering.scale) *
			mat3x3<float32>::rotation(descriptor.textureFiltering.rotation) *
			mat3x3<float32>::translation(descriptor.textureFiltering.translation);

		objects.emplace_back(object);
	}

	StaticScene3D StaticSceneBuilder::build() const {
		std::vector<StaticTriangle3D> triangles;
		std::vector<svec3<float32>> vertices;
		std::vector<svec3<float32>> normals;
		std::vector<svec2<float32>> textureCoordinates;

		uint32 vertexPositionIndexOffset = 0;
		uint32 vertexNormalIndexOffset = 0;
		uint32 vertexTextureCoordinateIndexOffset = 0;

		for (const StaticObject& object : objects) {
			const mat4x4<float32> world = (
				mat4x4<float32>::scaling(object.transform.scale) *
				mat4x4<float32>::translation(object.transform.position)
			);
			const view<float32> vertexPositionComponents = object.geometryAsset->getVertexPositionComponents();

			uint32 vertexOffset = 0;

			while (vertexOffset < vertexPositionComponents.count()) {
				const vec3<float32> vertex = multiplyTransform({
					vertexPositionComponents[vertexOffset + 0],
					vertexPositionComponents[vertexOffset + 1],
					vertexPositionComponents[vertexOffset + 2],
				}, world);

				vertices.emplace_back(vertex);

				vertexOffset += 3;
			}

			const view<float32> vertexNormalComponents = object.geometryAsset->getVertexNormalComponents();
			const view<float32> vertexTextureCoordinateComponents = object.geometryAsset->getVertexTextureCoordinateComponents();
			const view<uint32> vertexPositionIndices = object.geometryAsset->getVertexPositionIndices();
			const view<uint32> vertexNormalIndices = object.geometryAsset->getVertexNormalIndices();
			const view<uint32> vertexTextureCoordinateIndices = object.geometryAsset->getVertexTextureCoordinateIndices();

			uint32 indexOffset = 0;

			while (indexOffset < vertexPositionIndices.count()) {
				const uint32 normalIndex = vertexNormalIndices[indexOffset];

				ENGINE_ASSERT(
					normalIndex == vertexNormalIndices[indexOffset + 1] &&
					normalIndex == vertexNormalIndices[indexOffset + 2],
					"Static scenes must use face normals."
				);

				const uint32 aUvIndex = vertexTextureCoordinateIndices[indexOffset + 0];
				const uint32 bUvIndex = vertexTextureCoordinateIndices[indexOffset + 1];
				const uint32 cUvIndex = vertexTextureCoordinateIndices[indexOffset + 2];

				triangles.emplace_back(StaticTriangle3D{
					.aIndex = vertexPositionIndexOffset + vertexPositionIndices[indexOffset + 0],
					.bIndex = vertexPositionIndexOffset + vertexPositionIndices[indexOffset + 1],
					.cIndex = vertexPositionIndexOffset + vertexPositionIndices[indexOffset + 2],
					.normalIndex = vertexNormalIndexOffset + normalIndex,
					.aUvIndex = vertexTextureCoordinateIndexOffset + aUvIndex,
					.bUvIndex = vertexTextureCoordinateIndexOffset + bUvIndex,
					.cUvIndex = vertexTextureCoordinateIndexOffset + cUvIndex,
				});

				normals.emplace_back(svec3<float32>{
					vertexNormalComponents[normalIndex + 0],
					vertexNormalComponents[normalIndex + 1],
					vertexNormalComponents[normalIndex + 2],
				});

				textureCoordinates.emplace_back(svec2<float32>{vertexTextureCoordinateComponents[aUvIndex + 0], vertexTextureCoordinateComponents[aUvIndex + 1]});
				textureCoordinates.emplace_back(svec2<float32>{vertexTextureCoordinateComponents[bUvIndex + 0], vertexTextureCoordinateComponents[bUvIndex + 1]});
				textureCoordinates.emplace_back(svec2<float32>{vertexTextureCoordinateComponents[cUvIndex + 0], vertexTextureCoordinateComponents[cUvIndex + 1]});

				indexOffset += 3;
			}

			vertexPositionIndexOffset += indexOffset;
			vertexNormalIndexOffset += indexOffset;
			vertexTextureCoordinateIndexOffset += indexOffset;
		}

		// TODO(asset-builder/static-scene): Optimize space for multiple objects.
		DEBUG_ASSERT(objects.size() == 1);

		StaticScene3D staticScene{};
		staticScene.triangles = sequence<StaticTriangle3D>(triangles.size());
		staticScene.vertices = sequence<svec3<float32>>(vertices.size());
		staticScene.normals = sequence<svec3<float32>>(normals.size());
		staticScene.textureCoordinates = sequence<svec2<float32>>(textureCoordinates.size());

		std::copy(triangles.begin(), triangles.end(), &staticScene.triangles[0]);
		std::copy(vertices.begin(), vertices.end(), &staticScene.vertices[0]);
		std::copy(normals.begin(), normals.end(), &staticScene.normals[0]);
		std::copy(textureCoordinates.begin(), textureCoordinates.end(), &staticScene.textureCoordinates[0]);

		return staticScene;
	}

	/* uint16 StaticSceneBuilder::findVertex(svec3<float32> vertex) const {
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
	} */
}