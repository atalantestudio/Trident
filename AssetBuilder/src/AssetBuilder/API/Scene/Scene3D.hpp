// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Light/PointLight.hpp"
#include "Engine/API/Material/Material.hpp"
#include "Engine/ECS/Component/GeometryComponent.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"
#include "Engine/ECS/EntityComponentMapping.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"

#include "AssetBuilder/API/Geometry/Geometry3D.hpp"
#include "AssetBuilder/API/Scene/Scene3DBuildFlags.hpp"
#include "AssetBuilder/API/Transform.hpp"

namespace Trident {
	struct SpawnDescriptor {
		svec3<float32> position;

		/// In Euler angles.
		svec3<float32> direction;
	};

	struct Scene3DDescriptor {
		Scene3DBuildFlags flags;
		svec3<float32> skyColor;
		ref<ImageAsset> skyboxImageAsset;
		svec3<float32> sunDirection;
		svec3<float32> sunColor;
		float32 sunIntensity;
		SpawnDescriptor spawn;
	};

	struct BuildTriangle3D {
		uint32 index;
		uint32 lightmapCoordinateIndexA;
		uint32 lightmapCoordinateIndexB;
		uint32 lightmapCoordinateIndexC;
		uint32 materialIndex;
	};

	struct BuildGeometry {
		range<uint32, uint32> vertexRange;
		range<uint32, uint32> indexRange;
		uint32 assetIdentifier;
	};

	struct BuildMaterial {
		// TODO(filesystem/asset/material): Material assets are too small and could be merged with other materials or even baked into the scene.
		uint32 assetIdentifier;

		/// Affected range in the material geometry array.
		range<uint32, uint32> materialGeometryRange;
	};

	struct BuildMaterialShader {
		// TODO(tsl): Replace by an identifier once TSL is ready.
		ShaderModuleAsset asset;

		/// Affected range in the material array.
		range<uint32, uint32> materialRange;
	};

	struct BuildStaticSceneGeometry3D {
		uint32 geometryCount;
		uint32 bakedVertexCount;
		uint32 externalVertexCount;
		uint32 bakedIndexCount;
		uint32 externalIndexCount;

		/// Baked and external indices for LINE_LIST primitives.
		uint32 lineIndexCount;

		/// Used to visualize face wireframes.
		sequence<uint32> lineIndices;

		/// Baked coordinates, then coordinates from external assets.
		sequence<svec2<float32>> lightmapCoordinates;

		sequence<BuildTransform3D> instances;
		ImageAssetDescriptor lightmapAtlasImageAssetDescriptor;
	};

	struct BuildDynamicSceneGeometry3D {
		uint32 materialShaderOffset;
		sequence<PointLight> pointLights;
		uint32 entityCount;
		sequence<EntityComponentMapping<GeometryComponent>> geometryMappings;
		sequence<EntityComponentMapping<RigidBody3DComponent>> rigidBodyMappings;
	};

	struct BuildScene3D {
		svec3<float32> skyColor;
		ref<ImageAsset> environmentMapImageAsset;
		svec3<float32> sunDirection;

		/// Pre-multiplied by the sun intensity.
		svec3<float32> sunColor;

		sequence<BuildVertex3D> vertices;
		sequence<uint32> indices;

		sequence<BuildGeometry> geometries;

		/// Static and dynamic material shaders.
		sequence<BuildMaterialShader> materialShaders;

		/// Grouped by material shader.
		sequence<BuildMaterial> materials;

		/// Grouped by material.
		sequence<MaterialGeometry> materialGeometries;

		BuildStaticSceneGeometry3D staticGeometry;
		BuildDynamicSceneGeometry3D dynamicGeometry;

		SpawnDescriptor spawn;
	};
}