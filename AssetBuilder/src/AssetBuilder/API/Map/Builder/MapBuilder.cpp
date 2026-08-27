// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"

namespace Trident {
	MapBuilder::MapBuilder(const MapDescriptor& descriptor) :
		descriptor(descriptor)
	{}

	Face MapBuilder::createFace(view<FaceVertex> vertices, svec3<float32> normal) {
		Face face(this);
		face.normal = normal;

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
		scene.vertexCount = 0;
		scene.triangleCount = 0;
		scene.staticGeometry.vertexCount = 0;
		scene.staticGeometry.bakedVertices = {};
		scene.staticGeometry.bakedIndices = {};
		scene.staticGeometry.lightmapCoordinates = {};
		scene.staticGeometry.instances = {};
		scene.staticGeometry.materialShaders = {};
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.identifier = max<uint32>();
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.version = {0, 0, 0};
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.extent = {0, 0, 0};
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.mipLevelCount = 1;
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.layerCount = 1;
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.data = {};
		scene.dynamicGeometry.materialShaders = {};
		scene.dynamicGeometry.pointLights = {};
		scene.dynamicGeometry.entityCount = 0;
		scene.dynamicGeometry.geometryMappings = {};
		scene.dynamicGeometry.rigidBodyMappings = {};

		return scene;
	}
}