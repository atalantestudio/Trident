#pragma once

#include "ProjectA/Renderer/RendererAPI.hpp"

namespace ProjectA {
	class RenderCommand {
		private:
			static scope<RendererAPI> rendererApi;

		public:
			static void initialize() {
				rendererApi->initialize();
			}

			static void setViewport(std::int32_t x, std::int32_t y, std::uint32_t width, std::uint32_t height) {
				rendererApi->setViewport(x, y, width, height);
			}

			static void setClearColor(const lyah::vec4<float32>& color) {
				rendererApi->setClearColor(color);
			}

			static void clear() {
				rendererApi->clear();
			}

			static void drawIndexed(const ref<RenderPipeline>& renderPipeline, uint32 indexCount) {
				rendererApi->drawIndexed(renderPipeline, indexCount);
			}

			// TODO: RenderPassCommandEncoder
			// void draw(const uint32 vertexCount, const uint32 instanceCount, const uint32 vertexOffset);
	};
}