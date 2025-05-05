#pragma once

#include "Engine/Renderer/Font/TrueType/TrueTypeFont.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class TextRenderer : public Renderer {
		public:
			TextRenderer(const TrueTypeFont& font);

			// TODO: Use std::string_view instead of const std::string&.
			void setText(const std::string& text, float32 fontSize);

			void render(const ref<CommandEncoder>& commandEncoder) const final override;

			void createGlyphMapImageView();
			void createGlyphMapImageSampler();
			void createVertexBuffer();
			void createIndexBuffer();
			void createCameraBuffer();
			void createPipeline();

		private:
			void updateCamera() const;

		private:
			struct Vertex {
				vec2<float32> position;
				vec2<float32> uv;
			};

			struct Camera {
				mat3x3<float32> projection;
			};

		private:
			static constexpr uint32 MAX_QUADS = 64;

		private:
			const TrueTypeFont& font;

			ref<Buffer> vertexBuffer;
			Vertex* vertices = nullptr;

			ref<Buffer> indexBuffer;

			ref<Buffer> cameraBuffer;
			Camera* cameraMappedRange = nullptr;

			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Image> glyphMapImage;
			ref<ImageView> glyphMapImageView;

			ref<Sampler> glyphMapImageSampler;

			uint32 quadCount = 0;
	};
}