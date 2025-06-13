#pragma once

#include "Engine/API/Event/WindowEvent.hpp"
#include "Engine/ECS/Component/GUIComponent.hpp"
#include "Engine/GUI/ComponentStyle/ButtonStyle.hpp"
#include "Engine/GUI/TextureArrayLayer.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class GUIRenderer : public Renderer {
		public:
			void build();

			// Copies the image data to the texture array staging buffer.
			// Returns the layer index of the image in the array.
			uint32 registerTextureArrayLayer(const TextureArrayLayer& layer);

			void flushTextureArrayStagingBuffer() const;

			Extent2D getTextureArrayLayerExtent(uint32 layerIndex) const;

			void registerSubcomponent(const mat3x3<float32>& world, const mat3x3<float32>& imageWorld, uint32 imageIndex, const vec4<float32>& color);

			void registerGlyphSubcomponent(const mat3x3<float32>& world, const mat3x3<float32>& imageWorld, uint32 imageIndex, const TextStyle& textStyle);

			void render(const ref<CommandEncoder>& commandEncoder) const final override;

			void resize(WindowResizeEvent& event) final override;

			void resetCounters();

		private:
			void updateColorAttachment();

			void initializeVertexBuffer();
			void initializeIndexBuffer();
			void initializeSubcomponentBuffer();
			void initializeCameraBuffer();
			void initializeRenderPipeline();

			void updateCamera() const;

		private:
			struct Vertex {
				svec2<float32> vertex;
				svec2<float32> imageCoordinate;
			};

			struct Subcomponent {
				mat3x3<float32> world;
				mat3x3<float32> imageWorld;
				uint32 imageIndex;
				vec4<float32> color;
			};

			struct GlyphSubcomponent {
				mat3x3<float32> world;
				mat3x3<float32> textureWorld;
				uint32 fontIndex;
				float32 outlineWidth;
				vec4<float32> color;
				vec4<float32> outlineColor;
			};

		public:
			static constexpr uint32 TEXTURE_ARRAY_MAX_WIDTH = 2048;
			static constexpr uint32 TEXTURE_ARRAY_MAX_HEIGHT = 2048;

		private:
			static constexpr uint32 TEXTURE_ARRAY_MAX_LAYER_COUNT = 4;
			static constexpr uint64 TEXTURE_ARRAY_LAYER_BPP = sizeof(uint32);

			static constexpr uint32 MAX_SUBCOMPONENTS = 64;
			static constexpr uint32 MAX_GLYPH_SUBCOMPONENTS = 64;

		private:
			ref<RenderPass> renderPass;

			ref<Image> colorImage;
			ref<ImageView> colorImageView;

			ref<PipelineLayout> subcomponentPipelineLayout;
			ref<RenderPipeline> subcomponentRenderPipeline;

			ref<PipelineLayout> glyphSubcomponentPipelineLayout;
			ref<RenderPipeline> glyphSubcomponentRenderPipeline;

			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;

			ref<Buffer> subcomponentBuffer;
			Subcomponent* subcomponentMappedRange;

			ref<Buffer> glyphSubcomponentBuffer;
			GlyphSubcomponent* glyphSubcomponentMappedRange;

			ref<Buffer> cameraBuffer;
			mat3x3<float32>* cameraMappedRange;

			ref<Image> textureArray;
			ref<ImageView> textureArrayView;
			ref<Buffer> textureArrayStagingBuffer;
			uint8* textureArrayStagingBufferMappedRange;
			uint32 textureArrayLayerCount = 0;
			std::array<Extent2D, TEXTURE_ARRAY_MAX_LAYER_COUNT> textureArrayLayerExtents;

			ref<Sampler> sampler;
			ref<Sampler> bilinearSampler;

			uint32 subcomponentCount = 0;
			uint32 glyphSubcomponentCount = 0;
	};
}