// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Instance.hpp"
#include "Engine/API/Light/PointLight.hpp"
#include "Engine/API/Material/Material.hpp"
#include "Engine/API/Material/MaterialShader.hpp"
#include "Engine/ECS/Component/GeometryComponent.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"
#include "Engine/ECS/EntityComponentMapping.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"
#include "Engine/Filesystem/Asset/Geometry3DAsset.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"

namespace Trident {
	struct Scene3DAsset : public Asset {
		svec3<float32> skyColor;
		ref<ImageAsset> skyboxImageAsset;
		svec3<float32> sunDirection;
		svec3<float32> sunColor;

		sequence<uint8> geometryData;

		range<uint64, uint64> vertexRange;
		range<uint64, uint64> indexRange;
		range<uint64, uint64> faceLineIndexRange;
		range<uint64, uint64> lightmapCoordinateRange;

		uint64 staticVertexSize;
		uint64 staticPositionSize;
		uint64 staticNormalSize;
		uint64 staticTextureCoordinateSize;
		sequence<Instance3D> instances;
		sequence<ref<Geometry3DAsset>> geometryAssets;
		sequence<range<uint32, uint32>> geometryIndexRanges;
		sequence<MaterialGeometry> materialGeometries;
		sequence<Material> materials;
		sequence<MaterialShader> materialShaders;
		sequence<PointLight> pointLights;
		uint32 dynamicMaterialShaderOffset;
		ref<ImageAsset> lightmapAtlasImageAsset;
		uint32 entityCount;
		sequence<EntityComponentMapping<GeometryComponent>> geometryMappings;
		sequence<EntityComponentMapping<RigidBody3DComponent>> rigidBodyMappings;
		uint32 hullCount = 0;
		uint32 hullEdgeCount = 0;
		uint32 hullFaceCount = 0;
		uint16 hullFaceEdgeIndexCount = 0;
		uint32 hullLineIndexCount = 0;
		uint32 rigidBodyFacePlaneCount = 0;

		svec3<float32> spawnPosition;
		svec3<float32> spawnDirection;
	};
}