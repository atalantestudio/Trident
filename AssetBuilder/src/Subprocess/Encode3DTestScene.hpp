// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/ECS/Component/GeometryComponent.hpp"
#include "Engine/ECS/EntityComponentMapping.hpp"

#include "RigidBody3DComponent.hpp"

namespace Trident {
	inline ByteStream<StreamOperation::WRITE> encode3DTestScene(uint32 identifier) {
		const uint32 entityCount = 2;

		const sequence<uint32> geometryAssetIdentifiers = {
			// TODO(debug-only): lemon_1k
			2118110974,
		};

		const sequence<EntityComponentMapping<GeometryComponent>> geometryMappings = {
			{0, {0}},
			{1, {0}},
		};

		const sequence<EntityComponentMapping<RigidBody3DComponent>> rigidBodyMappings = {
			{
				0,
				createRigidBody({
					.position = {0.3_m, 0.0_m, 1.25_m},
					.rotation = {-pi<float32>() * 0.5f, -0.2_rad, pi<float32>()},
					.scale = {10.0f, 10.0f, 10.0f},
				}),
			},
			{
				1,
				createRigidBody({
					.position = {-0.35_m, -0.2_m, 1.25_m},
					.rotation = {-0.3_rad, -0.3_rad, pi<float32>() * 0.5f},
					.scale = {10.0f, 10.0f, 10.0f},
				}),
			},
		};

		SceneAssetHeader header{};
		header.signature = AssetSignature::SCENE;
		header.identifier = identifier;
		header.version = {0, 0, 0};
		header.staticVertexCount = 0;
		header.staticIndexCount = 0;
		header.entityCount = entityCount;
		header.geometryAssetCount = static_cast<uint32>(geometryAssetIdentifiers.count());
		header.geometryMappingCount = static_cast<uint32>(geometryMappings.count());
		header.rigidBodyMappingCount = static_cast<uint32>(rigidBodyMappings.count());

		uint64 size = 0;
		size += sizeof(header);
		size += header.staticVertexCount * sizeof(svec2<float32>);
		size += header.staticIndexCount * sizeof(uint16);
		size += header.geometryAssetCount * sizeof(uint32);
		size += header.geometryMappingCount * (sizeof(Entity) + sizeof(uint32));
		size += header.rigidBodyMappingCount * (sizeof(Entity) + sizeof(svec2<float32>) * 2 + sizeof(float32) * 5);

		ByteStream<StreamOperation::WRITE> stream(size, Endianness::LITTLE_ENDIAN);

		std::copy_n(pointer_cast<const uint8*>(&header), sizeof(header), stream.getBytePosition());

		stream.jump(sizeof(header));

		for (const uint32 geometryAssetIdentifier : geometryAssetIdentifiers) {
			stream.write<uint32>(geometryAssetIdentifier);
		}

		for (const EntityComponentMapping<GeometryComponent> geometryMapping : geometryMappings) {
			stream.write<Entity>(geometryMapping.entity);
			stream.write<uint32>(geometryMapping.component.geometryIndex);
		}

		for (const EntityComponentMapping<RigidBody3DComponent>& rigidBodyMapping : rigidBodyMappings) {
			stream.write<Entity>(rigidBodyMapping.entity);
			stream.write<svec3<float32>>(svec3<float32>(rigidBodyMapping.component.position));
			stream.write<svec3<float32>>(svec3<float32>(rigidBodyMapping.component.rotation));
			stream.write<svec3<float32>>(svec3<float32>(rigidBodyMapping.component.scale));
		}

		return stream;
	}
}