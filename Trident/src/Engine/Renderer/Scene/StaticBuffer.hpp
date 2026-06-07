// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Scene/BaseSceneRenderer.hpp"

namespace Trident {
	ref<Buffer<uint8>> BaseSceneRenderer::createStaticBuffer() const {
		const ref<Device> device = getDevice();
		const ref<Scene3DAsset> sceneAsset = scene->getAsset();
		const uint64 minStorageBufferOffsetAlignment = device->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;
		const uint64 triangleRangeSize = sceneAsset->getStaticTriangles().count() * sizeof(StaticTriangle3D);
		const uint64 vertexRangeSize = sceneAsset->getStaticVertices().count() * sizeof(svec3<float32>);
		const uint64 normalRangeSize = sceneAsset->getStaticNormals().count() * sizeof(svec3<float32>);
		const uint64 textureCoordinateRangeSize = sceneAsset->getStaticTextureCoordinates().count() * sizeof(svec2<float32>);
		const uint64 triangleAlignedRangeSize = ceil(triangleRangeSize, minStorageBufferOffsetAlignment);
		const uint64 vertexAlignedRangeSize = ceil(vertexRangeSize, minStorageBufferOffsetAlignment);
		const uint64 normalAlignedRangeSize = ceil(normalRangeSize, minStorageBufferOffsetAlignment);
		const uint64 textureCoordinateAlignedRangeSize = ceil(textureCoordinateRangeSize, minStorageBufferOffsetAlignment);

		return createBuffer<uint8>({
			.size = triangleAlignedRangeSize + vertexAlignedRangeSize + normalAlignedRangeSize + textureCoordinateAlignedRangeSize,
			.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::TRANSFER_DST,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Static",
			#endif
		});
	}

	ref<Buffer<uint8>> BaseSceneRenderer::createStagingStaticBuffer() const {
		const ref<Device> device = getDevice();
		const ref<Scene3DAsset> sceneAsset = scene->getAsset();
		const uint64 minStorageBufferOffsetAlignment = device->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;
		const uint64 triangleRangeSize = sceneAsset->getStaticTriangles().count() * sizeof(StaticTriangle3D);
		const uint64 vertexRangeSize = sceneAsset->getStaticVertices().count() * sizeof(svec3<float32>);
		const uint64 normalRangeSize = sceneAsset->getStaticNormals().count() * sizeof(svec3<float32>);
		const uint64 textureCoordinateRangeSize = sceneAsset->getStaticTextureCoordinates().count() * sizeof(svec2<float32>);
		const ref<Buffer<uint8>> staticStagingBuffer = createStagingBuffer<uint8>(triangleRangeSize + vertexRangeSize + normalRangeSize + textureCoordinateRangeSize);

		uint8* mappedRange = staticStagingBuffer->map(0, staticStagingBuffer->getSize());

		std::copy(scene->getAsset()->getStaticTriangles().begin(), scene->getAsset()->getStaticTriangles().end(), pointer_cast<StaticTriangle3D*>(mappedRange));
		mappedRange += triangleRangeSize;

		std::copy(scene->getAsset()->getStaticVertices().begin(), scene->getAsset()->getStaticVertices().end(), pointer_cast<svec3<float32>*>(mappedRange));
		mappedRange += vertexRangeSize;

		std::copy(scene->getAsset()->getStaticNormals().begin(), scene->getAsset()->getStaticNormals().end(), pointer_cast<svec3<float32>*>(mappedRange));
		mappedRange += normalRangeSize;

		std::copy(scene->getAsset()->getStaticTextureCoordinates().begin(), scene->getAsset()->getStaticTextureCoordinates().end(), pointer_cast<svec2<float32>*>(mappedRange));
		mappedRange += textureCoordinateRangeSize;

		staticStagingBuffer->unmap();

		return staticStagingBuffer;
	}

	void BaseSceneRenderer::copyStaticBuffer(ref<CommandEncoder> commandEncoder, ref<Buffer<uint8>> staticStagingBuffer) const {
		const ref<Device> device = getDevice();
		const ref<Scene3DAsset> sceneAsset = scene->getAsset();
		const uint64 minStorageBufferOffsetAlignment = device->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;
		const uint64 triangleRangeSize = sceneAsset->getStaticTriangles().count() * sizeof(StaticTriangle3D);
		const uint64 vertexRangeSize = sceneAsset->getStaticVertices().count() * sizeof(svec3<float32>);
		const uint64 normalRangeSize = sceneAsset->getStaticNormals().count() * sizeof(svec3<float32>);
		const uint64 textureCoordinateRangeSize = sceneAsset->getStaticTextureCoordinates().count() * sizeof(svec2<float32>);
		const uint64 triangleAlignedRangeSize = ceil(triangleRangeSize, minStorageBufferOffsetAlignment);
		const uint64 vertexAlignedRangeSize = ceil(vertexRangeSize, minStorageBufferOffsetAlignment);
		const uint64 normalAlignedRangeSize = ceil(normalRangeSize, minStorageBufferOffsetAlignment);
		const uint64 textureCoordinateAlignedRangeSize = ceil(textureCoordinateRangeSize, minStorageBufferOffsetAlignment);

		const sequence<BufferCopyRegion> bufferCopyRegions = {
			// Triangles
			{
				.sourceOffset = 0,
				.destinationOffset = 0,
				.size = triangleRangeSize,
			},

			// Vertices
			{
				.sourceOffset = triangleRangeSize,
				.destinationOffset = triangleAlignedRangeSize,
				.size = vertexRangeSize,
			},

			// Normals
			{
				.sourceOffset = triangleRangeSize + vertexRangeSize,
				.destinationOffset = triangleAlignedRangeSize + vertexAlignedRangeSize,
				.size = normalRangeSize,
			},

			// Texture coordinates
			{
				.sourceOffset = triangleRangeSize + vertexRangeSize + normalRangeSize,
				.destinationOffset = triangleAlignedRangeSize + vertexAlignedRangeSize + normalAlignedRangeSize,
				.size = textureCoordinateRangeSize,
			},
		};

		commandEncoder->copyBufferToBuffer(staticStagingBuffer, staticBuffer, bufferCopyRegions);
	}

	/* // hall_floor
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
	positionMappedRange += 4; */

	/* // hall_floor
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
	textureCoordinateMappedRange += 4; */
}