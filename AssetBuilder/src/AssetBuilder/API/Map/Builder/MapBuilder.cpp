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

	Face MapBuilder::createFace(view<FaceVertex> vertices, svec3<float32> normal) {
		Face face(this);
		face.normal = normal;
		face.indices.resize(vertices.count());

		uint32 indexOffset = 0;

		for (const FaceVertex& vertex : vertices) {
			facePositions[facePositionCount] = vertex.position;
			faceTextureCoordinates[faceTextureCoordinateCount] = vertex.textureCoordinate;

			face.indices[indexOffset] = indexOffset;

			facePositionCount += 1;
			faceTextureCoordinateCount += 1;
			indexOffset += 1;
		}

		faceIndexCount += indexOffset;

		return face;
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
		scene.vertexCount = facePositionCount;

		// TODO(asset-builder/map)
		scene.triangleCount = facePositionCount;

		scene.staticGeometry.vertexCount = facePositionCount;
		scene.staticGeometry.bakedVertices = sequence<BuildVertex3D>(facePositionCount);

		uint32 positionIndex = 0;

		while (positionIndex < facePositionCount) {
			scene.staticGeometry.bakedVertices[positionIndex].position = facePositions[positionIndex];

			positionIndex += 1;
		}

		scene.staticGeometry.bakedIndices = {0, 1, 2, 3, 4};
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