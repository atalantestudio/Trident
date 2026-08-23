// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Instance.hpp"
#include "Engine/API/Light/PointLight.hpp"
#include "Engine/API/Material/Material.hpp"
#include "Engine/API/Material/MaterialShader.hpp"
#include "Engine/API/Scene/StaticTriangle2D.hpp"
#include "Engine/ECS/Component/GeometryComponent.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"
#include "Engine/ECS/EntityComponentMapping.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"
#include "Engine/Filesystem/Asset/GeometryAsset.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"

namespace Trident {
	struct Scene2DAsset : public Asset {
		sequence<uint8> staticGeometryBuffer;
		sequence<ref<Geometry2DAsset>> geometryAssets;
		sequence<StaticTriangle2D> staticTriangles;
		sequence<svec2<float32>> staticVertices;
		sequence<svec2<float32>> staticTextureCoordinates;

		/// Also contains the line indices of the hulls.
		uint64 dynamicVertexPositionCount;

		uint64 dynamicVertexTextureCoordinateCount;
		uint64 dynamicVertexPositionIndexCount;
		uint64 dynamicVertexTextureCoordinateIndexCount;
		uint32 entityCount;
		uint32 hullCount;
		sequence<EntityComponentMapping<GeometryComponent>> geometryMappings;
		sequence<EntityComponentMapping<RigidBody2DComponent>> rigidBodyMappings;
	};

	struct Scene3DAsset : public Asset {
		sequence<uint8> geometryData;

		/// Vertices, indices, lightmap coordinates.
		range<uint64, uint64> geometryRanges[3];

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
		svec3<float32> skyColor;
		svec3<float32> sunDirection;
		uint32 entityCount;
		sequence<EntityComponentMapping<GeometryComponent>> geometryMappings;
		sequence<EntityComponentMapping<RigidBody3DComponent>> rigidBodyMappings;
		uint32 hullCount = 0;
		uint32 hullEdgeCount = 0;
		uint32 hullFaceCount = 0;
		uint16 hullFaceEdgeIndexCount = 0;
		uint32 hullLineIndexCount = 0;
		uint32 rigidBodyFacePlaneCount = 0;
	};
}