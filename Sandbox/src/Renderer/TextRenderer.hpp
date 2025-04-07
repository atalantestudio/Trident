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
			static constexpr uint32 MAX_QUAD_COUNT = 64;

		private:
			ref<Buffer> vertexBuffer;
			Vertex* vertices = nullptr;

			ref<Buffer> indexBuffer;

			ref<Buffer> cameraBuffer;
			Camera* camera;

			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Image> glyphMapImage;
			ref<ImageView> glyphMapImageView;
			ref<Sampler> glyphMapImageSampler;

			const TrueTypeFont& font;

			uint32 quadCount = 0;
	};
}