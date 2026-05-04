// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<Buffer<svec3<float32>>> ForwardRenderer::createStaticPositionBuffer() const {
		const ref<Buffer<svec3<float32>>> positionBuffer = createBuffer<svec3<float32>>({
			.size = 20 * sizeof(svec3<float32>),
			.usage = BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Static position",
			#endif
		});

		svec3<float32>* positionMappedRange = positionBuffer->map(0, positionBuffer->getSize());

		// hall_floor
		positionMappedRange[0] = {-16.0f,  0.0f, -16.0f };
		positionMappedRange[1] = {-16.0f,  0.0f,  184.0f};
		positionMappedRange[2] = { 128.0f, 0.0f,  184.0f};
		positionMappedRange[3] = { 128.0f, 0.0f, -16.0f };
		positionMappedRange += 4;

		// hall_to_bike_room_floor_connector
		positionMappedRange[0] = {-24.0f,  0.0f,  104.0f};
		positionMappedRange[1] = {-24.0f,  0.0f,  152.0f};
		positionMappedRange[2] = {-16.0f,  0.0f,  152.0f};
		positionMappedRange[3] = {-16.0f,  0.0f,  104.0f};
		positionMappedRange += 4;

		// bike_room_floor
		positionMappedRange[0] = {-96.0f,  0.0f,  48.0f };
		positionMappedRange[1] = {-96.0f,  0.0f,  184.0f};
		positionMappedRange[2] = {-24.0f,  0.0f,  184.0f};
		positionMappedRange[3] = {-24.0f,  0.0f,  48.0f };
		positionMappedRange += 4;

		// hall_to_main_floor_connector
		positionMappedRange[0] = { 24.0f,  0.0f,  184.0f};
		positionMappedRange[1] = { 24.0f,  0.0f,  192.0f};
		positionMappedRange[2] = { 84.0f,  0.0f,  192.0f};
		positionMappedRange[3] = { 84.0f,  0.0f,  184.0f};
		positionMappedRange += 4;

		// main_floor
		positionMappedRange[0] = {-168.0f, 0.0f,  192.0f};
		positionMappedRange[1] = {-168.0f, 0.0f,  320.0f};
		positionMappedRange[2] = { 128.0f, 0.0f,  320.0f};
		positionMappedRange[3] = { 128.0f, 0.0f,  192.0f};
		positionMappedRange += 4;

		positionBuffer->unmap();

		return positionBuffer;
	}

	ref<Buffer<svec3<float32>>> ForwardRenderer::createStaticNormalBuffer() const {
		const ref<Buffer<svec3<float32>>> normalBuffer = createBuffer<svec3<float32>>({
			.size = 1 * sizeof(svec3<float32>),
			.usage = BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Static normal",
			#endif
		});

		svec3<float32>* normalMappedRange = normalBuffer->map(0, normalBuffer->getSize());

		normalMappedRange[0] = {0.0f, 1.0f, 0.0f};
		normalMappedRange += 1;

		normalBuffer->unmap();

		return normalBuffer;
	}

	ref<Buffer<svec2<float32>>> ForwardRenderer::createStaticTextureCoordinateBuffer() const {
		const ref<Buffer<svec2<float32>>> textureCoordinateBuffer = createBuffer<svec2<float32>>({
			.size = 20 * sizeof(svec2<float32>),
			.usage = BufferUsage::READ_ONLY_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Static texture coordinate",
			#endif
		});

		svec2<float32>* textureCoordinateMappedRange = textureCoordinateBuffer->map(0, textureCoordinateBuffer->getSize());
		mat3x3<float32> texture;

		// hall_floor
		texture = mat3x3<float32>::scaling({-0.25f, 0.25f}) * mat3x3<float32>::rotation(0.0f) * mat3x3<float32>::translation({0.25f, 0.625f});
		textureCoordinateMappedRange[0] = svec2<float32>(multiplyTransform({0.0f, 0.0f}, texture));
		textureCoordinateMappedRange[1] = svec2<float32>(multiplyTransform({1.0f, 0.0f}, texture));
		textureCoordinateMappedRange[2] = svec2<float32>(multiplyTransform({0.0f, 1.0f}, texture));
		textureCoordinateMappedRange[3] = svec2<float32>(multiplyTransform({1.0f, 1.0f}, texture));
		textureCoordinateMappedRange += 4;

		// hall_to_bike_room_floor_connector
		texture = mat3x3<float32>::scaling({-0.25f, 0.25f}) * mat3x3<float32>::rotation(0.0f) * mat3x3<float32>::translation({0.375f, 0.125f});
		textureCoordinateMappedRange[0] = svec2<float32>(multiplyTransform({0.0f, 0.0f}, texture));
		textureCoordinateMappedRange[1] = svec2<float32>(multiplyTransform({1.0f, 0.0f}, texture));
		textureCoordinateMappedRange[2] = svec2<float32>(multiplyTransform({0.0f, 1.0f}, texture));
		textureCoordinateMappedRange[3] = svec2<float32>(multiplyTransform({1.0f, 1.0f}, texture));
		textureCoordinateMappedRange += 4;

		// bike_room_floor
		texture = mat3x3<float32>::scaling({-0.25f, 0.25f}) * mat3x3<float32>::rotation(0.0f) * mat3x3<float32>::translation({0.5f, 0.624f});
		textureCoordinateMappedRange[0] = svec2<float32>(multiplyTransform({0.0f, 0.0f}, texture));
		textureCoordinateMappedRange[1] = svec2<float32>(multiplyTransform({1.0f, 0.0f}, texture));
		textureCoordinateMappedRange[2] = svec2<float32>(multiplyTransform({0.0f, 1.0f}, texture));
		textureCoordinateMappedRange[3] = svec2<float32>(multiplyTransform({1.0f, 1.0f}, texture));
		textureCoordinateMappedRange += 4;

		// hall_to_main_floor_connector
		texture = mat3x3<float32>::scaling({-0.25f, 0.25f}) * mat3x3<float32>::rotation(0.0f) * mat3x3<float32>::translation({0.625f, 0.5f});
		textureCoordinateMappedRange[0] = svec2<float32>(multiplyTransform({0.0f, 0.0f}, texture));
		textureCoordinateMappedRange[1] = svec2<float32>(multiplyTransform({1.0f, 0.0f}, texture));
		textureCoordinateMappedRange[2] = svec2<float32>(multiplyTransform({0.0f, 1.0f}, texture));
		textureCoordinateMappedRange[3] = svec2<float32>(multiplyTransform({1.0f, 1.0f}, texture));
		textureCoordinateMappedRange += 4;

		// main_floor
		texture = mat3x3<float32>::scaling({-0.25f, 0.25f}) * mat3x3<float32>::rotation(0.0f) * mat3x3<float32>::translation({0.625f, 0.5f});
		textureCoordinateMappedRange[0] = svec2<float32>(multiplyTransform({0.0f, 0.0f}, texture));
		textureCoordinateMappedRange[1] = svec2<float32>(multiplyTransform({1.0f, 0.0f}, texture));
		textureCoordinateMappedRange[2] = svec2<float32>(multiplyTransform({0.0f, 1.0f}, texture));
		textureCoordinateMappedRange[3] = svec2<float32>(multiplyTransform({1.0f, 1.0f}, texture));
		textureCoordinateMappedRange += 4;

		textureCoordinateBuffer->unmap();

		return textureCoordinateBuffer;
	}

	ref<Buffer<ForwardRenderer::PackedVertex>> ForwardRenderer::createStaticVertexBuffer() const {
		const ref<Buffer<ForwardRenderer::PackedVertex>> vertexBuffer = createBuffer<ForwardRenderer::PackedVertex>({
			.size = 20 * sizeof(ForwardRenderer::PackedVertex),
			.usage = BufferUsage::VERTEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Static vertex",
			#endif
		});

		PackedVertex* vertexMappedRange = vertexBuffer->map(0, vertexBuffer->getSize());
		uint16 positionOffset = 0;
		uint16 textureCoordinateOffset = 0;

		// hall_floor
		vertexMappedRange[0] = {(uint16) (positionOffset + 0), 0, (uint16) (textureCoordinateOffset + 0)};
		vertexMappedRange[1] = {(uint16) (positionOffset + 1), 0, (uint16) (textureCoordinateOffset + 1)};
		vertexMappedRange[2] = {(uint16) (positionOffset + 2), 0, (uint16) (textureCoordinateOffset + 2)};
		vertexMappedRange[3] = {(uint16) (positionOffset + 3), 0, (uint16) (textureCoordinateOffset + 3)};
		vertexMappedRange += 4;
		positionOffset += 4;
		textureCoordinateOffset += 4;

		// hall_to_bike_room_floor_connector
		vertexMappedRange[0] = {(uint16) (positionOffset + 0), 0, (uint16) (textureCoordinateOffset + 0)};
		vertexMappedRange[1] = {(uint16) (positionOffset + 1), 0, (uint16) (textureCoordinateOffset + 1)};
		vertexMappedRange[2] = {(uint16) (positionOffset + 2), 0, (uint16) (textureCoordinateOffset + 2)};
		vertexMappedRange[3] = {(uint16) (positionOffset + 3), 0, (uint16) (textureCoordinateOffset + 3)};
		vertexMappedRange += 4;
		positionOffset += 4;
		textureCoordinateOffset += 4;

		// bike_room_floor
		vertexMappedRange[0] = {(uint16) (positionOffset + 0), 0, (uint16) (textureCoordinateOffset + 0)};
		vertexMappedRange[1] = {(uint16) (positionOffset + 1), 0, (uint16) (textureCoordinateOffset + 1)};
		vertexMappedRange[2] = {(uint16) (positionOffset + 2), 0, (uint16) (textureCoordinateOffset + 2)};
		vertexMappedRange[3] = {(uint16) (positionOffset + 3), 0, (uint16) (textureCoordinateOffset + 3)};
		vertexMappedRange += 4;
		positionOffset += 4;
		textureCoordinateOffset += 4;

		// hall_to_main_floor_connector
		vertexMappedRange[0] = {(uint16) (positionOffset + 0), 0, (uint16) (textureCoordinateOffset + 0)};
		vertexMappedRange[1] = {(uint16) (positionOffset + 1), 0, (uint16) (textureCoordinateOffset + 1)};
		vertexMappedRange[2] = {(uint16) (positionOffset + 2), 0, (uint16) (textureCoordinateOffset + 2)};
		vertexMappedRange[3] = {(uint16) (positionOffset + 3), 0, (uint16) (textureCoordinateOffset + 3)};
		vertexMappedRange += 4;
		positionOffset += 4;
		textureCoordinateOffset += 4;

		// main_floor
		vertexMappedRange[0] = {(uint16) (positionOffset + 0), 0, (uint16) (textureCoordinateOffset + 0)};
		vertexMappedRange[1] = {(uint16) (positionOffset + 1), 0, (uint16) (textureCoordinateOffset + 1)};
		vertexMappedRange[2] = {(uint16) (positionOffset + 2), 0, (uint16) (textureCoordinateOffset + 2)};
		vertexMappedRange[3] = {(uint16) (positionOffset + 3), 0, (uint16) (textureCoordinateOffset + 3)};
		vertexMappedRange += 4;
		positionOffset += 4;
		textureCoordinateOffset += 4;

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