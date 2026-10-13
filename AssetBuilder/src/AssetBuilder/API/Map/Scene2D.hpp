// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Scene/StaticTriangle2D.hpp"
#include "Engine/ECS/Component/Geometry2DComponent.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"
#include "Engine/ECS/EntityComponentMapping.hpp"

namespace Trident {
	struct BuildStaticSceneGeometry2D {
		sequence<StaticTriangle2D> triangles;
		sequence<vec2<float32>> vertices;
		sequence<vec2<float32>> textureCoordinates;
	};

	struct BuildDynamicSceneGeometry2D {
		uint32 entityCount;
		sequence<uint32> geometryAssetIdentifiers;
		sequence<EntityComponentMapping<Geometry2DComponent>> geometryMappings;
		sequence<EntityComponentMapping<RigidBody2DComponent>> rigidBodyMappings;
	};

	struct BuildScene2D {
		BuildStaticSceneGeometry2D staticGeometry;
		BuildDynamicSceneGeometry2D dynamicGeometry;
	};
}