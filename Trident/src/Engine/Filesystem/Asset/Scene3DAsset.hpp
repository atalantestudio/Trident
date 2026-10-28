// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Hull3D.hpp"
#include "Engine/API/Geometry/Instance.hpp"
#include "Engine/API/Light/PointLight.hpp"
#include "Engine/API/Material/Material.hpp"
#include "Engine/API/Material/MaterialShader.hpp"
#include "Engine/ECS/Component/Geometry3DComponent.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"
#include "Engine/ECS/EntityComponentMapping.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"
#include "Engine/Filesystem/Asset/Geometry3DAsset.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"

namespace Trident {
	struct Scene3DAsset : public Asset {
		vec3<float32> skyColor;
		ref<ImageAsset> skyboxImageAsset;
		vec3<float32> sunDirection;
		vec3<float32> sunColor;

		sequence<uint8> geometryData;

		range<uint64, uint64> vertexRange;
		range<uint64, uint64> indexRange;
		range<uint64, uint64> faceLineIndexRange;
		range<uint64, uint64> lightmapCoordinateRange;

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
		sequence<EntityComponentMapping<Geometry3DComponent>> geometryMappings;
		sequence<EntityComponentMapping<RigidBody3DComponent>> rigidBodyMappings;
		uint32 rigidBodyFacePlaneCount;
		uint32 entityInstanceOffset;

		vec3<float32> spawnPosition;
		vec3<float32> spawnDirection;
	};
}