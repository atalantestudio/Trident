// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/ECS/Component/GeometryComponent.hpp"
#include "Engine/ECS/EntityComponentMapping.hpp"

#include "RigidBody2DComponent.hpp"

namespace Trident {
	inline ByteStream<StreamOperation::WRITE> encode2DTestScene(uint32 identifier) {
		const uint32 entityCount = 2;

		const sequence<uint32> geometryAssetIdentifiers = {
			// TODO(debug-only)
			484634187,
		};

		const sequence<EntityComponentMapping<GeometryComponent>> geometryMappings = {
			{0, {0}},
			{1, {0}},
		};

		const sequence<EntityComponentMapping<RigidBody2DComponent>> rigidBodyMappings = {
			{
				0,
				createRigidBody({
					.position = {0.0_m, -10.0_m},
					.rotation = 0.0_rad,
					.scale = {100.0f, 20.0f},
					.mass = max<float32>(),
					.friction = 0.2f,
					.restitution = 0.0f,
					.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
				}),
			},
			{
				1,
				createRigidBody({
					.position = {0.0_m, 4.0_m},
					.rotation = 0.0_rad,
					.scale = {1.0f, 1.0f},
					.mass = 200.0_kg,
					.friction = 0.2f,
					.restitution = 0.0f,
					.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
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
		size += header.staticIndexCount * sizeof(uint32);
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

		for (const EntityComponentMapping<RigidBody2DComponent>& rigidBodyMapping : rigidBodyMappings) {
			stream.write<Entity>(rigidBodyMapping.entity);
			stream.write<svec2<float32>>(svec2<float32>(rigidBodyMapping.component.position));
			stream.write<float32>(rigidBodyMapping.component.rotation);
			stream.write<svec2<float32>>(svec2<float32>(rigidBodyMapping.component.scale));
			stream.write<float32>(rigidBodyMapping.component.inverseMass);
			stream.write<float32>(rigidBodyMapping.component.inverseInertia);
			stream.write<float32>(rigidBodyMapping.component.friction);
			stream.write<float32>(rigidBodyMapping.component.restitution);
		}

		return stream;
	}
}