// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/Scene3DAsset.hpp"
#include "Engine/Renderer/3D/SceneRenderer.hpp"

namespace Trident {
	class DebugRenderer : public SceneRenderer<Scene3DAsset> {
		private:
			struct Camera {
				mat4x4<float32> projectionView;
				vec3<float32> position;
			};

			struct SkyboxCamera {
				mat4x4<float32> projectionView;
			};

		public:
			void build() override;

			void postSetScene() override;

			void postSetCameraController() override {}

			void encodeOneTimeCommands() const override {}

			void render(ref<CommandEncoder> commandEncoder) const override;

			void resize(WindowResizeEvent& event) override;

			void updateCamera() const;

			void createFrameObjects();

		private:
			ref<RenderPass> createRenderPass() const;

			void createFramebuffers();

			ref<DescriptorSetLayout> createSkyboxDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createGeometryDescriptorSetLayout() const;

			ref<PipelineLayout> createSkyboxPipelineLayout() const;
			ref<PipelineLayout> createGeometryPipelineLayout() const;

			ref<GraphicsPipeline> createSkyboxPipeline() const;
			ref<GraphicsPipeline> createGeometryFillPipeline() const;
			ref<GraphicsPipeline> createGeometryLinePipeline() const;

			ref<Image> createSkyboxImage(ref<ImageAsset> imageAsset) const;
			ref<Image> createDepthStencilImage() const;

			ref<ImageView> createSkyboxImageView() const;
			ref<ImageView> createDepthStencilImageView() const;

			ref<Buffer> createSkyboxCameraBuffer() const;
			ref<Buffer> createCameraBuffer() const;
			ref<Buffer> createVertexBuffer() const;
			ref<Buffer> createIndexBuffer() const;

			ref<Sampler> createSkyboxSampler() const;

			void updateDescriptorSets() const;

			void encodeSceneOneTimeCommands() const;

			void encodeSkyboxPass(ref<CommandEncoder> commandEncoder) const;
			void encodeGeometryFillPass(ref<CommandEncoder> commandEncoder) const;
			void encodeGeometryLinePass(ref<CommandEncoder> commandEncoder) const;

		private:
			ref<RenderPass> renderPass;

			sequence<ref<Framebuffer>> framebuffers;

			ref<DescriptorSetLayout> skyboxDescriptorSetLayout;
			ref<DescriptorSetLayout> geometryDescriptorSetLayout;

			sequence<ref<DescriptorSet>> descriptorSets;

			ref<PipelineLayout> skyboxPipelineLayout;
			ref<PipelineLayout> geometryPipelineLayout;

			ref<GraphicsPipeline> skyboxPipeline;
			ref<GraphicsPipeline> geometryFillPipeline;
			ref<GraphicsPipeline> geometryLinePipeline;

			ref<Buffer> skyboxCameraBuffer;
			ref<Buffer> cameraBuffer;
			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;

			ref<Image> skyboxImage;
			ref<Image> depthStencilImage;

			ref<ImageView> skyboxImageView;
			ref<ImageView> depthStencilImageView;

			ref<Sampler> skyboxSampler;

			BufferView<SkyboxCamera> skyboxCameraView;
			BufferView<Camera> cameraView;

			SkyboxCamera* skyboxCameraMappedRange = nullptr;
			Camera* cameraMappedRange = nullptr;
	};
}