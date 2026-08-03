// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Transform.hpp"

#include "Engine/API/Geometry/Index.hpp"
#include "Engine/ECS/Component/GeometryComponent.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"
#include "Engine/ECS/EntityComponentMapping.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"

namespace Trident {
	struct BuildGeometry {
		range<uint32, uint32> vertexRange;
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
		sequence<GeometryIndex3D> bakedVertices;
		sequence<svec3<float32>> bakedPositions;
		sequence<svec3<float32>> bakedNormals;
		sequence<svec2<float32>> bakedTextureCoordinates;

		/// Baked coordinates, then coordinates from external assets.
		sequence<svec2<float32>> lightmapCoordinates;

		sequence<BuildTransform3D> instances;

		/// These are variants of the Lightmapped shader.
		sequence<BuildMaterialShader> materialShaders;

		ImageAssetDescriptor lightmapAtlasImageAssetDescriptor;
	};

	struct BuildDynamicSceneGeometry3D {
		/// These are variants of the Forward shader.
		sequence<BuildMaterialShader> materialShaders;

		uint32 entityCount;
		sequence<EntityComponentMapping<GeometryComponent>> geometryMappings;
		sequence<EntityComponentMapping<RigidBody3DComponent>> rigidBodyMappings;
	};

	struct BuildScene3D {
		sequence<BuildGeometry> geometries;

		/// Grouped by material.
		sequence<BuildMaterialGeometry> materialGeometries;

		/// Grouped by material shader.
		sequence<BuildMaterial> materials;

		uint32 triangleCount;
		uint32 positionCount;
		uint32 normalCount;
		uint32 textureCoordinateCount;

		BuildStaticSceneGeometry3D staticGeometry;
		BuildDynamicSceneGeometry3D dynamicGeometry;

		svec3<float32> skyColor;
	};
}