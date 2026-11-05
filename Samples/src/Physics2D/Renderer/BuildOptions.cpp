// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Physics2D/Renderer/Renderer.hpp"

namespace Trident {
	void Physics2DRenderer::build(const PhysicsRendererBuildOptions2D& options) {
		build();

		const uint32 vertexCount = options.maxVertexCount + 4;
		const uint32 transformCount = options.maxTransformCount;
		const uint32 indexCount = vertexCount / 2 * 3;

		vertexBuffer = createVertexBuffer(vertexCount);
		transformBuffer = createTransformBuffer(transformCount);
		indexBuffer = createIndexBuffer(indexCount);
	}

	StaticMemoryDescriptor Physics2DRenderer::getStaticMemoryDescriptor() const {
		return {
			.buffers = view<ref<Buffer>>(&vertexBuffer, 3),
			.images = {},
		};
	}
}