// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Experimental/GI2D/CameraBuffer.hpp"
#include "Experimental/GI2D/CameraDescriptorSetLayout.hpp"
#include "Experimental/GI2D/CameraProjection.hpp"
#include "Experimental/GI2D/DescriptorSet.hpp"
#include "Experimental/GI2D/Framebuffer.hpp"
#include "Experimental/GI2D/GlobalIlluminationPipeline.hpp"
#include "Experimental/GI2D/GlobalIlluminationPipelineLayout.hpp"
#include "Experimental/GI2D/Renderer.hpp"
#include "Experimental/GI2D/RenderPass.hpp"
#include "Experimental/GI2D/VertexBuffer.hpp"
#include "Experimental/GI2D/VertexGenerationDescriptorSetLayout.hpp"
#include "Experimental/GI2D/VertexGenerationPipeline.hpp"
#include "Experimental/GI2D/VertexGenerationPipelineLayout.hpp"

namespace Trident {
	void GI2DExperimentalRenderer::build() {
		renderPass = createRenderPass();
		descriptorSetLayouts[0] = createVertexGenerationDescriptorSetLayout();
		descriptorSetLayouts[1] = createCameraDescriptorSetLayout();
		descriptorSets = getDevice()->allocateDescriptorSets(descriptorSetLayouts);
		vertexGenerationPipelineLayout = createVertexGenerationPipelineLayout();
		globalIlluminationPipelineLayout = createGlobalIlluminationPipelineLayout();
		vertexGenerationPipeline = createVertexGenerationPipeline();
		globalIlluminationPipeline = createGlobalIlluminationPipeline();
		buffers[0] = createVertexBuffer();
		vertexBufferView = buffers[0]->createView<Vertex>(0, buffers[0]->getSize());
		buffers[1] = createCameraBuffer();
		cameraBufferView = buffers[1]->createView<Camera>(0, buffers[1]->getSize());

		createFramebuffers();
	}

	StaticMemoryDescriptor GI2DExperimentalRenderer::getStaticMemoryDescriptor() const {
		return {
			.buffers = buffers,
			.images = {},
		};
	}

	void GI2DExperimentalRenderer::postBuild() {
		cameraMappedRange = cameraBufferView.map();

		updateDescriptorSets();
		updateCameraProjection();
	}

	void GI2DExperimentalRenderer::render(ref<CommandEncoder> commandEncoder) const {
		const ref<Framebuffer> framebuffer = framebuffers[getSwapchainImageIndex()];
		const Extent2D extent = framebuffer->getExtent();
		const vec2<float32> size = vec2<float32>(extent);

		commandEncoder->bindComputePipeline(vertexGenerationPipeline);
		commandEncoder->bindDescriptorSets(vertexGenerationPipelineLayout, 0, descriptorSets);
		commandEncoder->dispatch(1, 1, 1);

		commandEncoder->beginRenderPass({
			.framebuffer = framebuffer,
			.clearValues = {{0.3f, 0.3f, 0.3f, 1.0f}},
		});
		commandEncoder->setViewport({0.0f, size.y}, {size.x, -size.y}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, extent);
		commandEncoder->bindGraphicsPipeline(globalIlluminationPipeline);
		commandEncoder->bindVertexBuffer(0, buffers[0], 0);
		commandEncoder->bindDescriptorSets(globalIlluminationPipelineLayout, 0, descriptorSets[1]);
		commandEncoder->draw(6, 1, 0, 0);
		commandEncoder->endRenderPass();
	}

	void GI2DExperimentalRenderer::resize(WindowResizeEvent& event) {
		createFramebuffers();
		updateCameraProjection();
	}
}