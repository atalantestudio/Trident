// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
		void ForwardRenderer::updateGIDescriptorSets() const {
			const BufferView<ForwardCamera> cameraBufferView = cameraBuffer->createView<ForwardCamera>(0, cameraBuffer->getSize());
			const BufferView<Vertex3D> vertexBufferView = vertexBuffer->createView<Vertex3D>(0, vertexBuffer->getSize());
			const BufferView<uint32> indexBufferView = indexBuffer->createView<uint32>(0, indexBuffer->getSize());
			const BufferView<GIVertex> giVertexBufferView = giVertexBuffer->createView<GIVertex>(0, giVertexBuffer->getSize());

			const WriteDescriptorSet writeDescriptorSets[] = {
				// Camera
				{
					.descriptorSet = giVertexGenerationDescriptorSet,
					.bindingIndex = 0,
					.descriptorBufferViews = cameraBufferView,
				},

				// Vertex
				{
					.descriptorSet = giVertexGenerationDescriptorSet,
					.bindingIndex = 1,
					.descriptorBufferViews = vertexBufferView,
				},

				// Index
				{
					.descriptorSet = giVertexGenerationDescriptorSet,
					.bindingIndex = 2,
					.descriptorBufferViews = indexBufferView,
				},

				// Point light
				{
					.descriptorSet = giVertexGenerationDescriptorSet,
					.bindingIndex = 3,
					.descriptorBufferViews = pointLightBufferView,
				},

				// GI vertex
				{
					.descriptorSet = giVertexGenerationDescriptorSet,
					.bindingIndex = 4,
					.descriptorBufferViews = giVertexBufferView,
				},

				// GI indirect
				{
					.descriptorSet = giVertexGenerationDescriptorSet,
					.bindingIndex = 5,
					.descriptorBufferViews = giIndirectBufferView,
				},
			};

			getDevice()->updateDescriptorSets(writeDescriptorSets);
		}
	#endif
}