// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/API/Physics/Cached/CachedGeometry2D.hpp"
#include "Engine/API/Physics/Cached/CachedRigidBody2D.hpp"
#include "Engine/API/Process/Process.hpp"

namespace Trident {
	// TODO(assets/geometry-3d): Convert to 3D.
	inline ByteStream<StreamOperation::WRITE> encode3DTestScene(uint32 identifier) {
		std::vector<float32> dynamicVertexComponents;
		std::vector<uint16> dynamicIndices;

		const uint32 entityCount = 2;

		const sequence<CachedGeometry2D> geometries = {
			CachedGeometry2D::unitSquare(0, dynamicVertexComponents, dynamicIndices),
		};

		const sequence<GeometryComponent> geometryComponents = {
			{
				.geometryIndex = 0,
			},
			{
				.geometryIndex = 0,
			},
		};

		const sequence<CachedRigidBody2D> rigidBodies = {
			CachedRigidBody2D({
				.entity = 0,
				.position = {0.0_m, -10.0_m},
				.rotation = 0.0_rad,
				.scale = {100.0f, 20.0f},
				.mass = max<float32>(),
				.friction = 0.2f,
			}),
			CachedRigidBody2D({
				.entity = 1,
				.position = {0.0_m, 4.0_m},
				.rotation = 0.0_rad,
				.scale = {1.0f, 1.0f},
				.mass = 200.0_kg,
				.friction = 0.2f,
			}),
		};

		Scene2DAssetHeader header{};
		header.signature = AssetSignature::SCENE;
		header.identifier = identifier;
		header.version = {0, 0, 0};
		header.staticVertexCount = 0;
		header.staticIndexCount = 0;
		header.dynamicVertexCount = static_cast<uint32>(dynamicVertexComponents.size()) / 2;
		header.dynamicIndexCount = static_cast<uint32>(dynamicIndices.size());
		header.entityCount = entityCount;
		header.geometryCount = static_cast<uint32>(geometries.count());
		header.rigidBodyCount = static_cast<uint32>(rigidBodies.count());
		header.geometryComponentCount = static_cast<uint32>(geometryComponents.count());

		uint64 size = 0;
		size += sizeof(header);
		size += header.staticVertexCount * sizeof(svec2<float32>);
		size += header.staticIndexCount * sizeof(uint16);
		size += header.dynamicVertexCount * sizeof(svec2<float32>);
		size += header.dynamicIndexCount * sizeof(uint16);
		size += header.geometryCount * (sizeof(Entity) + sizeof(svec2<float32>) + sizeof(float32) + sizeof(uint32) * 3 + sizeof(uint16) * 2 + sizeof(Geometry2DType));
		size += header.rigidBodyCount * (sizeof(Entity) + sizeof(svec2<float32>) * 2 + sizeof(float32) * 4);
		size += header.geometryComponentCount * sizeof(GeometryComponent);

		ByteStream<StreamOperation::WRITE> stream(size, Endianness::LITTLE_ENDIAN);

		std::copy_n(pointer_cast<const uint8*>(&header), sizeof(header), stream.getBytePosition());
		stream.jump(sizeof(header));

		std::copy_n(pointer_cast<const uint8*>(&dynamicVertexComponents[0]), dynamicVertexComponents.size() * sizeof(float32), stream.getBytePosition());
		stream.jump(dynamicVertexComponents.size() * sizeof(float32));

		std::copy_n(pointer_cast<const uint8*>(&dynamicIndices[0]), dynamicIndices.size() * sizeof(uint16), stream.getBytePosition());
		stream.jump(dynamicIndices.size() * sizeof(uint16));

		for (const CachedGeometry2D& geometry : geometries) {
			stream.write<svec2<float32>>(geometry.centroid);
			stream.write<float32>(geometry.radius);
			stream.write<uint32>(geometry.entity);
			stream.write<uint32>(geometry.vertexOffset);
			stream.write<uint32>(geometry.vertexCount);
			stream.write<uint32>(geometry.indexOffset);
			stream.write<uint16>(geometry.triangleListIndexCount);
			stream.write<uint16>(geometry.lineStripIndexCount);
			stream.write<Geometry2DType>(geometry.type);
		}

		for (const CachedRigidBody2D& rigidBody : rigidBodies) {
			stream.write<svec2<float32>>(rigidBody.position);
			stream.write<svec2<float32>>(rigidBody.scale);
			stream.write<float32>(rigidBody.rotation);
			stream.write<float32>(rigidBody.inverseMass);
			stream.write<float32>(rigidBody.inverseInertia);
			stream.write<float32>(rigidBody.friction);
			stream.write<uint32>(rigidBody.entity);
		}

		for (const GeometryComponent geometryComponent : geometryComponents) {
			stream.write<GeometryComponent>(geometryComponent);
		}

		return stream;
	}
}