// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Event/Window/WindowResizeEvent.hpp"
#include "Engine/API/GUI/ID.hpp"
#include "Engine/API/GUI/TextureArrayLayer.hpp"
#include "Engine/API/GUI/Subcomponent.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	class GUIRenderer : public Renderer {
		private:
			struct Camera {
				mat3x3<float32> projection;
			};

			struct Vertex {
				svec2<float32> position;
				svec2<float32> textureCoordinate;
			};

		public:
			static constexpr uint32 MAX_USER_IMAGE_COUNT = 8;
			static constexpr uint32 MAX_SUBCOMPONENT_COUNT = 2048;
			static constexpr uint32 MAX_GLYPH_SUBCOMPONENT_COUNT = 2048;

		public:
			void build() final override;

			ref<Image> getUserImage(uint32 imageIndex) const;

			void registerDefaultUserImage();

			ImageID registerUserImage(ref<ImageAsset> imageAsset);

			void flushUserImageStagingBuffer();

			void registerSubcomponent(const GUISubcomponent& subcomponent);

			void registerGlyphSubcomponent(const GUIGlyphSubcomponent& glyphSubcomponent);

			void render(ref<CommandEncoder> commandEncoder) const final override;

			void resize(WindowResizeEvent& event) final override;

			void resetCounters();

			inline bool doneFlushing() const {
				return userImagesFlushed;
			}

		private:
			ref<RenderPass> createRenderPass() const;

			void recreateFramebuffers();

			ref<DescriptorSetLayout> createSubcomponentDescriptorSetLayout() const;

			void updateDescriptorSets() const;

			ref<PipelineLayout> createSubcomponentPipelineLayout() const;

			ref<GraphicsPipeline> createSubcomponentGraphicsPipeline() const;
			ref<GraphicsPipeline> createGlyphSubcomponentGraphicsPipeline() const;

			ref<Buffer<Vertex>> createVertexBuffer() const;
			ref<Buffer<uint16>> createIndexBuffer() const;
			ref<Buffer<Camera>> createCameraBuffer() const;
			ref<Buffer<uint8>> createUserImageStagingBuffer() const;
			ref<Buffer<GUISubcomponent>> createSubcomponentBuffer() const;
			ref<Buffer<GUIGlyphSubcomponent>> createGlyphSubcomponentBuffer() const;

			ref<Image> createDepthStencilImage() const;

			ref<ImageView> createDepthStencilImageView() const;

			ref<Sampler> createNearestSampler() const;
			ref<Sampler> createLinearSampler() const;

			void copyBuffers() const;

			void updateCameraProjection() const;

		private:
			ref<RenderPass> renderPass;

			sequence<ref<Framebuffer>> framebuffers;

			ref<DescriptorSetLayout> subcomponentDescriptorSetLayout;

			ref<DescriptorSet> subcomponentDescriptorSet;

			ref<PipelineLayout> subcomponentPipelineLayout;

			ref<GraphicsPipeline> subcomponentGraphicsPipeline;
			ref<GraphicsPipeline> glyphSubcomponentGraphicsPipeline;

			ref<Buffer<Camera>> cameraBuffer;
			ref<Buffer<Vertex>> vertexBuffer;
			ref<Buffer<uint16>> indexBuffer;
			ref<Buffer<GUISubcomponent>> subcomponentBuffer;
			ref<Buffer<GUIGlyphSubcomponent>> glyphSubcomponentBuffer;
			ref<Buffer<uint8>> userImageStagingBuffer;

			ref<Image> depthStencilImage;
			sequence<ref<Image>> userImages;

			ref<ImageView> depthStencilImageView;
			sequence<ref<ImageView>> userImageViews;

			ref<Sampler> nearestSampler;
			ref<Sampler> linearSampler;

			Camera* cameraBufferMappedRange = nullptr;
			GUISubcomponent* subcomponentBufferMappedRange = nullptr;
			GUIGlyphSubcomponent* glyphSubcomponentBufferMappedRange = nullptr;
			uint8* userImageStagingBufferMappedRange = nullptr;

			uint32 subcomponentCount = 0;
			uint32 glyphSubcomponentCount = 0;
			uint32 userImageCount = 0;

			bool userImagesFlushed = false;
	};
}