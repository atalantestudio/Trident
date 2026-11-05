// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Physics2D/Renderer/Renderer.hpp"

namespace Trident {
	void Physics2DRenderer::initializeGeometryBuffers(uint32 maxVertexCount, uint32 maxTransformCount) {
		DEBUG_ASSERT(vertexBuffer == nullptr && indexBuffer == nullptr);

		// Circles are rendered with 4 vertices.
		maxVertexCount += 4;

		vertexBuffer = createStaticBuffer({
			.size = maxVertexCount * sizeof(vec3<float32>),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Vertex",
			#endif
		});

		transformBuffer = createStaticBuffer({
			.size = maxTransformCount * sizeof(Transform2D),
			.usage = BufferUsage::VERTEX,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = true,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Transform",
			#endif
		});

		indexBuffer = createStaticBuffer({
			.size = maxVertexCount / 2 * 3,
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::INDEX,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Index",
			#endif
		});
	}

	void Physics2DRenderer::copyGeometry() const {
		// TODO
	}
}