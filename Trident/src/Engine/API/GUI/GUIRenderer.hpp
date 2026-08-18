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
			struct Vertex {
				svec2<float32> position;
				svec2<float32> textureCoordinate;
			};

			struct Camera {
				mat3x3<float32> projection;
			};

		public:
			static constexpr uint32 MAX_USER_IMAGE_COUNT = 8;
			static constexpr uint32 MAX_SUBCOMPONENT_COUNT = 2048;
			static constexpr uint32 MAX_GLYPH_SUBCOMPONENT_COUNT = 2048;

		public:
			void build() final override;

			void encodeOneTimeCommands() const override;

			void render(ref<CommandEncoder> commandEncoder) const final override;

			void resize(WindowResizeEvent& event) final override;

			bool doneFlushing() const;

			ref<Image> getUserImage(uint32 imageIndex) const;

			ImageID registerUserImage(ref<ImageAsset> imageAsset);

			void registerDefaultUserImage();

			void flushUserImageStagingBuffer();

			void registerSubcomponent(const GUISubcomponent& subcomponent);

			void registerGlyphSubcomponent(const GUIGlyphSubcomponent& glyphSubcomponent);

			void resetCounters();

		private:
			ref<DescriptorSetLayout> createSubcomponentDescriptorSetLayout() const;
			ref<PipelineLayout> createSubcomponentPipelineLayout() const;
			ref<GraphicsPipeline> createSubcomponentGraphicsPipeline() const;
			ref<GraphicsPipeline> createGlyphSubcomponentGraphicsPipeline() const;
			ref<Buffer> createCameraBuffer() const;
			ref<Buffer> createVertexBuffer() const;
			ref<Buffer> createIndexBuffer() const;
			ref<Buffer> createSubcomponentBuffer() const;
			ref<Buffer> createGlyphSubcomponentBuffer() const;
			ref<Buffer> createUserImageStagingBuffer() const;
			ref<Sampler> createNearestSampler() const;
			ref<Sampler> createLinearSampler() const;

			void updateDescriptorSets() const;
			void updateCameraProjection() const;

		private:
			ref<DescriptorSetLayout> subcomponentDescriptorSetLayout;
			ref<DescriptorSet> subcomponentDescriptorSet;
			ref<PipelineLayout> subcomponentPipelineLayout;
			ref<GraphicsPipeline> subcomponentGraphicsPipeline;
			ref<GraphicsPipeline> glyphSubcomponentGraphicsPipeline;
			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;
			ref<Buffer> cameraBuffer;
			ref<Buffer> subcomponentBuffer;
			ref<Buffer> glyphSubcomponentBuffer;

			/// Can store up to 8 2048x2048 RGBA textures.
			ref<Buffer> userImageStagingBuffer;

			ref<Image> userImages[MAX_USER_IMAGE_COUNT];
			ref<ImageView> userImageViews[MAX_USER_IMAGE_COUNT];
			ref<Sampler> nearestSampler;
			ref<Sampler> linearSampler;

			BufferView<Camera> cameraView;

			Camera* cameraMappedRange = nullptr;
			GUISubcomponent* subcomponentMappedRange = nullptr;
			GUIGlyphSubcomponent* glyphSubcomponentMappedRange = nullptr;
			uint8* userImageStagingMappedRange = nullptr;

			uint32 subcomponentCount = 0;
			uint32 glyphSubcomponentCount = 0;
			uint32 userImageCount = 0;
			bool userImagesFlushed = false;
	};
}