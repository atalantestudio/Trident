// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"

namespace Trident {
	MapBuilder::MapBuilder(const MapDescriptor& descriptor) :
		descriptor(descriptor),
		facePositions(MAX_FACE_POSITIONS),
		faceTextureCoordinates(MAX_FACE_TEXTURE_COORDINATES)
	{}

	Face& MapBuilder::createFace(view<FaceVertex> vertices, svec3<float32> normal) {
		Face face(this);
		face.normal = normal;
		face.indices.resize(vertices.count());

		uint32 indexOffset = 0;

		for (const FaceVertex& vertex : vertices) {
			facePositions[facePositionCount] = vertex.position;
			faceTextureCoordinates[faceTextureCoordinateCount] = vertex.textureCoordinate;

			face.indices[indexOffset] = indexOffset;

			facePositionCount += 1;
			faceVertexCount += 1;
			faceTextureCoordinateCount += 1;
			indexOffset += 1;
		}

		faces.emplace_back(face);

		faceIndexCount += indexOffset;
		triangleCount += indexOffset / 2 + indexOffset % 2;

		return faces.back();
	}

	BuildScene3D MapBuilder::build() {
		BuildScene3D scene{};
		scene.skyColor = {0.0f, 0.0f, 0.0f};

		if (descriptor.skyboxImageAsset != nullptr) {
			scene.skyboxAssetIdentifier = descriptor.skyboxImageAsset->identifier;
		}
		else {
			scene.skyboxAssetIdentifier = max<uint32>();
		}

		scene.sunDirection = {0.0f, 0.0f, 0.0f};
		scene.sunColor = {0.0f, 0.0f, 0.0f};
		scene.geometries = {};
		scene.materialGeometries = {};
		scene.materials = {};
		scene.vertexCount = faceVertexCount;
		scene.triangleCount = triangleCount;
		scene.staticGeometry.vertexCount = faceVertexCount;
		scene.staticGeometry.bakedVertices = sequence<BuildVertex3D>(faceVertexCount);

		const uint32 triangulatedIndexCount = triangleCount * 3;

		scene.staticGeometry.bakedIndices = sequence<uint32>(triangulatedIndexCount);

		uint32 faceIndex = 0;
		uint32 vertexOffset = 0;
		uint32 indexOffset = 0;

		while (faceIndex < faces.size()) {
			const Face& face = faces[faceIndex];
			const uint32 indexCount = static_cast<uint32>(face.indices.size());
			const uint32 triangleCount = indexCount / 2 + indexCount % 2;
			const uint32 triangulatedIndexCount = triangleCount * 3;

			uint32 vertexIndex = 0;

			while (vertexIndex < indexCount) {
				BuildVertex3D& vertex = scene.staticGeometry.bakedVertices[face.indices[vertexOffset + vertexIndex]];
				vertex.position = facePositions[vertexIndex];
				vertex.normal = face.normal;

				// TODO(asset-builder/map): Calculate face tangent and vertex texture coordinates.
				vertex.tangent = {0.0f, 0.0f, 0.0f, 0.0f};
				vertex.textureCoordinate = {0.0f, 0.0f};

				vertexIndex += 1;
			}

			uint32 triangleIndex = 0;

			while (triangleIndex < triangleCount) {
				// TODO(asset-builder/map): Faces are assumed to be convex and have their vertices laid out in a clockwise order.
				scene.staticGeometry.bakedIndices[indexOffset + triangleIndex * 3 + 0] = indexOffset;
				scene.staticGeometry.bakedIndices[indexOffset + triangleIndex * 3 + 1] = indexOffset + triangleIndex + 1;
				scene.staticGeometry.bakedIndices[indexOffset + triangleIndex * 3 + 2] = indexOffset + triangleIndex + 2;

				triangleIndex += 1;
			}

			vertexOffset += vertexIndex;
			indexOffset += triangulatedIndexCount;
			faceIndex += 1;
		}

		scene.staticGeometry.lightmapCoordinates = {};
		scene.staticGeometry.instances = {};
		scene.staticGeometry.materialShaders = {};
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.identifier = AssetLibrary::createAssetIdentifier();
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.version = {0, 0, 0};
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.extent = {1, 1, 1};
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.mipLevelCount = 1;
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.layerCount = 1;
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.data = {0, 0, 0, 1};
		scene.dynamicGeometry.materialShaders = {};
		scene.dynamicGeometry.pointLights = {};
		scene.dynamicGeometry.entityCount = 0;
		scene.dynamicGeometry.geometryMappings = {};
		scene.dynamicGeometry.rigidBodyMappings = {};

		return scene;
	}
}