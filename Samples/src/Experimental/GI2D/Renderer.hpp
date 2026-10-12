// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../../Graphics/GraphicsLayer.hpp"

namespace Trident {
	class GI2DExperimentalRenderer : public Renderer {
		public:
			struct Vertex {
				vec2<float32> position;
				vec2<float32> distanceA;
				vec2<float32> distanceB;
			};

			struct Camera {
				mat4x3<float32> projection;
			};

		public:
			void build() override;

			StaticMemoryDescriptor getStaticMemoryDescriptor() const final override;

			void postBuild() final override;

			void render(ref<CommandEncoder> commandEncoder) const final override;

			void resize(WindowResizeEvent& event) final override;

		private:
			ref<RenderPass> createRenderPass() const;

			void createFramebuffers();

			ref<DescriptorSetLayout> createVertexGenerationDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createCameraDescriptorSetLayout() const;

			ref<PipelineLayout> createVertexGenerationPipelineLayout() const;
			ref<PipelineLayout> createGlobalIlluminationPipelineLayout() const;

			ref<ComputePipeline> createVertexGenerationPipeline() const;
			ref<GraphicsPipeline> createGlobalIlluminationPipeline() const;

			ref<Buffer> createVertexBuffer() const;
			ref<Buffer> createCameraBuffer() const;

			void updateDescriptorSets() const;
			void updateCameraProjection() const;

		private:
			ref<RenderPass> renderPass;
			sequence<ref<Framebuffer>> framebuffers;

			ref<DescriptorSetLayout> descriptorSetLayouts[2];

			sequence<ref<DescriptorSet>> descriptorSets;

			ref<PipelineLayout> globalIlluminationPipelineLayout;
			ref<PipelineLayout> vertexGenerationPipelineLayout;

			ref<ComputePipeline> vertexGenerationPipeline;
			ref<GraphicsPipeline> globalIlluminationPipeline;

			// Vertex, camera
			ref<Buffer> buffers[2];

			BufferView<Vertex> vertexBufferView;
			BufferView<Camera> cameraBufferView;

			Camera* cameraMappedRange = nullptr;
	};

	DEFINE_SAMPLE_LAYER(GI2DExperimental);
}