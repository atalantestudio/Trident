// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<Buffer<svec3<float32>>> ForwardRenderer::createStaticVertexBuffer() const {
		const ref<Buffer<svec3<float32>>> vertexBuffer = createBuffer<svec3<float32>>({
			.size = 20 * sizeof(svec3<float32>),
			.usage = BufferUsage::VERTEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Static vertex",
			#endif
		});

		svec3<float32>* vertexMappedRange = vertexBuffer->map(0, vertexBuffer->getSize());

		// hall_floor
		vertexMappedRange[0] = {-16.0f,  0.0f, -16.0f };
		vertexMappedRange[1] = {-16.0f,  0.0f,  184.0f};
		vertexMappedRange[2] = { 128.0f, 0.0f,  184.0f};
		vertexMappedRange[3] = { 128.0f, 0.0f, -16.0f };
		vertexMappedRange += 4;

		// hall_to_bike_room_floor_connector
		vertexMappedRange[0] = {-24.0f,  0.0f,  104.0f};
		vertexMappedRange[1] = {-24.0f,  0.0f,  152.0f};
		vertexMappedRange[2] = {-16.0f,  0.0f,  152.0f};
		vertexMappedRange[3] = {-16.0f,  0.0f,  104.0f};
		vertexMappedRange += 4;

		// bike_room_floor
		vertexMappedRange[0] = {-96.0f,  0.0f,  48.0f };
		vertexMappedRange[1] = {-96.0f,  0.0f,  184.0f};
		vertexMappedRange[2] = {-24.0f,  0.0f,  184.0f};
		vertexMappedRange[3] = {-24.0f,  0.0f,  48.0f };
		vertexMappedRange += 4;

		// hall_to_main_floor_connector
		vertexMappedRange[0] = { 24.0f,  0.0f,  184.0f};
		vertexMappedRange[1] = { 24.0f,  0.0f,  192.0f};
		vertexMappedRange[2] = { 84.0f,  0.0f,  192.0f};
		vertexMappedRange[3] = { 84.0f,  0.0f,  184.0f};
		vertexMappedRange += 4;

		// main_floor
		vertexMappedRange[0] = {-168.0f, 0.0f,  192.0f};
		vertexMappedRange[1] = {-168.0f, 0.0f,  320.0f};
		vertexMappedRange[2] = { 128.0f, 0.0f,  320.0f};
		vertexMappedRange[3] = { 128.0f, 0.0f,  192.0f};
		vertexMappedRange += 4;

		vertexBuffer->unmap();

		return vertexBuffer;
	}

	ref<Buffer<uint16>> ForwardRenderer::createStaticIndexBuffer() const {
		const ref<Buffer<uint16>> indexBuffer = createBuffer<uint16>({
			.size = 30 * sizeof(uint16),
			.usage = BufferUsage::INDEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Static index",
			#endif
		});

		uint16* indexMappedRange = indexBuffer->map(0, indexBuffer->getSize());
		uint16 indexOffset = 0;

		// hall_floor
		indexMappedRange[0] = indexOffset + 0;
		indexMappedRange[1] = indexOffset + 1;
		indexMappedRange[2] = indexOffset + 2;
		indexMappedRange[3] = indexOffset + 0;
		indexMappedRange[4] = indexOffset + 2;
		indexMappedRange[5] = indexOffset + 3;
		indexMappedRange += 6;
		indexOffset += 4;

		// hall_to_bike_room_floor_connector
		indexMappedRange[0] = indexOffset + 0;
		indexMappedRange[1] = indexOffset + 1;
		indexMappedRange[2] = indexOffset + 2;
		indexMappedRange[3] = indexOffset + 0;
		indexMappedRange[4] = indexOffset + 2;
		indexMappedRange[5] = indexOffset + 3;
		indexMappedRange += 6;
		indexOffset += 4;

		// bike_room_floor
		indexMappedRange[0] = indexOffset + 0;
		indexMappedRange[1] = indexOffset + 1;
		indexMappedRange[2] = indexOffset + 2;
		indexMappedRange[3] = indexOffset + 0;
		indexMappedRange[4] = indexOffset + 2;
		indexMappedRange[5] = indexOffset + 3;
		indexMappedRange += 6;
		indexOffset += 4;

		// hall_to_main_floor_connector
		indexMappedRange[0] = indexOffset + 0;
		indexMappedRange[1] = indexOffset + 1;
		indexMappedRange[2] = indexOffset + 2;
		indexMappedRange[3] = indexOffset + 0;
		indexMappedRange[4] = indexOffset + 2;
		indexMappedRange[5] = indexOffset + 3;
		indexMappedRange += 6;
		indexOffset += 4;

		// main_floor
		indexMappedRange[0] = indexOffset + 0;
		indexMappedRange[1] = indexOffset + 1;
		indexMappedRange[2] = indexOffset + 2;
		indexMappedRange[3] = indexOffset + 0;
		indexMappedRange[4] = indexOffset + 2;
		indexMappedRange[5] = indexOffset + 3;
		indexMappedRange += 6;
		indexOffset += 4;

		indexBuffer->unmap();

		return indexBuffer;
	}
}