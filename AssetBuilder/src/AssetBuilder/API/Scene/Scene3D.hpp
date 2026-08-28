// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Geometry/Geometry3D.hpp"
#include "AssetBuilder/API/Scene/Scene3DBuildFlags.hpp"
#include "AssetBuilder/API/Transform.hpp"

#include "Engine/API/Light/PointLight.hpp"
#include "Engine/ECS/Component/GeometryComponent.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"
#include "Engine/ECS/EntityComponentMapping.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"

namespace Trident {
	struct Scene3DDescriptor {
		Scene3DBuildFlags flags;
		svec3<float32> skyColor;
		ref<ImageAsset> skyboxImageAsset;
		svec3<float32> sunDirection;
		svec3<float32> sunColor;
		float32 sunIntensity;
	};

	struct BuildGeometry {
		range<uint32, uint32> indexRange;
		uint32 assetIdentifier;
	};

	struct BuildMaterialGeometry {
		range<uint32, uint32> instanceRange;
		uint32 geometryIndex;
	};

	struct BuildMaterial {
		// TODO(filesystem/asset/material): Material assets are too small and could be merged with other materials or baked into the scene.
		uint32 assetIdentifier;

		/// Affected range in the geometry array.
		uint32 geometryCount;
	};

	struct BuildMaterialShader {
		// TODO(tsl): Replace by an identifier once TSL is ready.
		ShaderModuleAsset asset;

		/// Affected range in the material array.
		range<uint32, uint32> materialRange;
	};

	struct BuildStaticSceneGeometry3D {
		/// Baked and external vertices.
		uint32 vertexCount;

		/// Used to visualize face wireframes.
		uint32 lineIndexCount;

		sequence<BuildVertex3D> bakedVertices;
		sequence<uint32> bakedIndices;

		/// Used to visualize face wireframes.
		sequence<uint32> lineIndices;

		/// Baked coordinates, then coordinates from external assets.
		sequence<svec2<float32>> lightmapCoordinates;

		sequence<BuildTransform3D> instances;
		sequence<BuildMaterialShader> materialShaders;
		ImageAssetDescriptor lightmapAtlasImageAssetDescriptor;
	};

	struct BuildDynamicSceneGeometry3D {
		sequence<BuildMaterialShader> materialShaders;
		sequence<PointLight> pointLights;
		uint32 entityCount;
		sequence<EntityComponentMapping<GeometryComponent>> geometryMappings;
		sequence<EntityComponentMapping<RigidBody3DComponent>> rigidBodyMappings;
	};

	struct BuildScene3D {
		svec3<float32> skyColor;
		uint32 skyboxAssetIdentifier;
		svec3<float32> sunDirection;

		/// Pre-multiplied by the sun intensity.
		svec3<float32> sunColor;

		sequence<BuildGeometry> geometries;

		/// Grouped by material.
		sequence<BuildMaterialGeometry> materialGeometries;

		/// Grouped by material shader.
		sequence<BuildMaterial> materials;

		uint32 vertexCount;
		uint32 triangleCount;
		BuildStaticSceneGeometry3D staticGeometry;
		BuildDynamicSceneGeometry3D dynamicGeometry;
	};
}