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
			ref<DescriptorSetLayout> createSubcomponentDescriptorSetLayout() const;

			void updateDescriptorSets() const;

			ref<PipelineLayout> createSubcomponentPipelineLayout() const;

			ref<GraphicsPipeline> createSubcomponentGraphicsPipeline() const;
			ref<GraphicsPipeline> createGlyphSubcomponentGraphicsPipeline() const;

			ref<Buffer> createVertexBuffer() const;
			ref<Buffer> createIndexBuffer() const;
			ref<Buffer> createCameraBuffer() const;
			ref<Buffer> createUserImageStagingBuffer() const;
			ref<Buffer> createSubcomponentBuffer() const;
			ref<Buffer> createGlyphSubcomponentBuffer() const;

			ref<Sampler> createNearestSampler() const;
			ref<Sampler> createLinearSampler() const;

			void copyBuffers() const;

			void updateCameraProjection() const;

		private:
			ref<DescriptorSetLayout> subcomponentDescriptorSetLayout;

			ref<DescriptorSet> subcomponentDescriptorSet;

			ref<PipelineLayout> subcomponentPipelineLayout;

			ref<GraphicsPipeline> subcomponentGraphicsPipeline;
			ref<GraphicsPipeline> glyphSubcomponentGraphicsPipeline;

			ref<Buffer> cameraBuffer;
			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;
			ref<Buffer> subcomponentBuffer;
			ref<Buffer> glyphSubcomponentBuffer;
			ref<Buffer> userImageStagingBuffer;

			sequence<ref<Image>> userImages;

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