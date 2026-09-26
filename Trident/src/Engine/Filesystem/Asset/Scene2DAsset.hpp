// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Scene/StaticTriangle2D.hpp"
#include "Engine/ECS/Component/GeometryComponent.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"
#include "Engine/ECS/EntityComponentMapping.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"
#include "Engine/Filesystem/Asset/Geometry2DAsset.hpp"

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
}