#pragma once

#include "Engine/Renderer/Font/TrueType/TrueTypeFont.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class TextRenderer : public Renderer {
		public:
			TextRenderer(const TrueTypeFont& font);

			void setText(const std::string& text);

			void render(const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) override;

			void createGlyphMapImageView();
			void createGlyphMapImageSampler();
			void createVertexBuffer();
			void createIndexBuffer();
			void createCameraUniformBuffer();
			void createPipeline();

		private:
			void updateCamera() const;

		private:
			struct Vertex {
				vec2<float32> position;
				vec2<float32> uv;
			};

		private:
			static constexpr uint32 MAX_CHARACTERS = 64;

		private:
			const TrueTypeFont& font;
			uint32 characterCount;

			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;

			ref<Buffer> cameraUniformBuffer;
			mat3x3<float32>* cameraMappedRange;

			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Image> glyphMapImage;
			ref<ImageView> glyphMapImageView;
			ref<Sampler> glyphMapImageSampler;
	};
}