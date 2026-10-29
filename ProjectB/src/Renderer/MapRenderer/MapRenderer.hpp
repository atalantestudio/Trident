// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Camera/3D/PerspectiveCamera3D.hpp"
#include "Engine/API/Map/Map.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectB {
	using namespace Trident;

	class MapRenderer : public Renderer {
		private:
			struct CameraUniform {
				mat4x4<float32> projectionView;
				vec3<float32> position;
			};

		private:
			static constexpr ImageSampleCount SAMPLE_COUNT = ImageSampleCount::x1;

		public:
			void build() override;

			StaticMemoryDescriptor getStaticMemoryDescriptor() const override;

			void postBuild() override;

			void render(ref<CommandEncoder> commandEncoder) const override;

			void resize(WindowResizeEvent& event) override;

			void updateCamera(const PerspectiveCamera3D& camera) const;

			void loadMap(const Map& map) override;

		private:
			ref<RenderPass> createRenderPass() const;

			void createFramebuffers();

			ref<DescriptorSetLayout> createStaticGeometryDescriptorSetLayout() const;

			ref<PipelineLayout> createStaticGeometryPipelineLayout() const;

			ref<GraphicsPipeline> createStaticGeometryPipeline() const;

			ref<Buffer> createVertexBuffer() const;
			ref<Buffer> createIndexBuffer() const;
			ref<Buffer> createCameraBuffer() const;

			void createCameraBuffers();

			ref<Image> createDepthStencilImage() const;

			ref<ImageView> createDepthStencilImageView() const;

			void updateDescriptorSets() const;

			void encodeMapOneTimeCommands() const;

		private:
			Map map;
			bool loadedMap = false;

			ref<RenderPass> renderPass;

			sequence<ref<Framebuffer>> framebuffers;

			ref<DescriptorSetLayout> staticGeometryDescriptorSetLayout;

			sequence<ref<DescriptorSet>> staticGeometryDescriptorSets;

			ref<PipelineLayout> staticGeometryPipelineLayout;

			ref<GraphicsPipeline> staticGeometryPipeline;

			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;
			sequence<ref<Buffer>> cameraBuffers;

			ref<Image> depthStencilImage;

			ref<ImageView> depthStencilImageView;

			sequence<BufferView<CameraUniform>> cameraViews;

			sequence<CameraUniform*> cameraMappedRanges;
	};
}