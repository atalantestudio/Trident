#pragma once

#include "Engine/API/Event/Window/WindowResizeEvent.hpp"
#include "Engine/API/GUI/TextureArrayLayer.hpp"
#include "Engine/API/GUI/Subcomponent.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
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
			static constexpr uint32 MAX_SUBCOMPONENTS = 2048;
			static constexpr uint32 MAX_GLYPH_SUBCOMPONENTS = 2048;
			static constexpr uint32 VERTEX_BUFFER_SIZE = 6 * sizeof(Vertex);
			static constexpr uint32 INDEX_BUFFER_SIZE = 6 * sizeof(uint16);
			static constexpr uint32 SUBCOMPONENT_BUFFER_SIZE = MAX_SUBCOMPONENTS * sizeof(GUISubcomponent);
			static constexpr uint32 GLYPH_SUBCOMPONENT_BUFFER_SIZE = MAX_GLYPH_SUBCOMPONENTS * sizeof(GUIGlyphSubcomponent);
			static constexpr uint32 TEXTURE_ARRAY_MAX_WIDTH = 2048;
			static constexpr uint32 TEXTURE_ARRAY_MAX_HEIGHT = 2048;
			static constexpr uint32 TEXTURE_ARRAY_MAX_LAYER_COUNT = 8;
			static constexpr uint64 TEXTURE_ARRAY_LAYER_BPP = sizeof(uint32);
			static inline const vec2<float32> TEXTURE_ARRAY_MAX_SIZE = {TEXTURE_ARRAY_MAX_WIDTH, TEXTURE_ARRAY_MAX_HEIGHT};

		public:
			void build() final override;

			Extent2D getTextureArrayLayerExtent(uint32 layerIndex) const;

			void flushTextureArrayStagingBuffer() const;

			// Copies the image data to the texture array staging buffer.
			// Returns the layer index of the image in the array.
			uint32 registerTextureArrayLayer(const TextureArrayLayer& layer);

			void registerSubcomponent(const GUISubcomponent& subcomponent);

			void registerGlyphSubcomponent(const GUIGlyphSubcomponent& glyphSubcomponent);

			void render(ref<CommandEncoder> commandEncoder) const final override;

			void resize(WindowResizeEvent& event) final override;

			void resetCounters();

		private:
			void setupRenderPass();

			void recreateFramebuffers();

			void createCameraBuffer();
			void createVertexBuffer();
			void createIndexBuffer();
			void createSubcomponentBuffer();
			void createGlyphSubcomponentBuffer();
			void createTextureArrayStagingBuffer();
			void createTextureArrayImage();
			void createTextureArrayImageView();
			void createNearestSampler();
			void createLinearSampler();
			void createDescriptorSetLayouts();
			void createSubcomponentPipelineLayout();
			void createGlyphSubcomponentPipelineLayout();
			void createSubcomponentRenderPipeline();
			void createGlyphSubcomponentRenderPipeline();

			void allocateDescriptorSets();

			void copyBuffers() const;

			void updateDescriptorSets() const;

			void updateCameraProjection() const;

		private:
			ref<RenderPass> renderPass;

			sequence<ref<Framebuffer>> framebuffers;

			ref<DescriptorSetLayout> subcomponentDescriptorSetLayout;
			ref<DescriptorSetLayout> glyphSubcomponentDescriptorSetLayout;

			ref<DescriptorSet> subcomponentDescriptorSet;
			ref<DescriptorSet> glyphSubcomponentDescriptorSet;

			ref<PipelineLayout> subcomponentPipelineLayout;
			ref<PipelineLayout> glyphSubcomponentPipelineLayout;

			ref<RenderPipeline> subcomponentRenderPipeline;
			ref<RenderPipeline> glyphSubcomponentRenderPipeline;

			ref<Buffer> cameraBuffer;
			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;
			ref<Buffer> subcomponentBuffer;
			ref<Buffer> glyphSubcomponentBuffer;
			ref<Buffer> textureArrayStagingBuffer;

			ref<Image> textureArrayImage;

			ref<ImageView> textureArrayImageView;

			ref<Sampler> nearestSampler;
			ref<Sampler> linearSampler;

			Camera* cameraBufferMappedRange = nullptr;
			GUISubcomponent* subcomponentBufferMappedRange = nullptr;
			GUIGlyphSubcomponent* glyphSubcomponentBufferMappedRange = nullptr;
			uint8* textureArrayStagingBufferMappedRange = nullptr;

			std::array<Extent2D, TEXTURE_ARRAY_MAX_LAYER_COUNT> textureArrayLayerExtents;

			uint32 subcomponentCount = 0;
			uint32 glyphSubcomponentCount = 0;
			uint32 textureArrayLayerCount = 0;
	};
}