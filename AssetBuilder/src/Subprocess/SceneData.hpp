// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/ECS/Component/GeometryComponent.hpp"
#include "Engine/ECS/Component/MaterialComponent.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"
#include "Engine/ECS/EntityComponentMapping.hpp"

namespace Trident {
	struct Scene2DData {
		uint32 staticVertexCount;
		uint32 staticIndexCount;
		uint32 entityCount;
		sequence<uint32> geometryAssetIdentifiers;
		sequence<EntityComponentMapping<GeometryComponent>> geometryMappings;
		sequence<EntityComponentMapping<RigidBody2DComponent>> rigidBodyMappings;
	};

	struct Scene3DData {
		uint32 staticVertexCount;
		uint32 staticIndexCount;
		uint32 entityCount;
		sequence<uint32> geometryAssetIdentifiers;
		sequence<EntityComponentMapping<GeometryComponent>> geometryMappings;
		sequence<EntityComponentMapping<MaterialComponent>> materialMappings;
		sequence<EntityComponentMapping<RigidBody3DComponent>> rigidBodyMappings;
	};
}