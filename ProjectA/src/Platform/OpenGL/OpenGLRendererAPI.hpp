#pragma once

#include <glad/glad.h>

#include "Engine/Renderer/RendererAPI.hpp"

namespace ProjectA {
	class OpenGLRendererAPI : public RendererAPI {
		public:
			virtual void initialize() override;

			virtual void setViewport(std::int32_t x, std::int32_t, std::uint32_t width, std::uint32_t height) const override;

			virtual void setClearColor(const lyah::vec4<float>& color) const override;

			virtual void clear() const override;

			virtual void drawIndexed(const ref<RenderPipeline>& renderPipeline, uint32 indexCount) const override;

		private:
			#ifdef BUILD_MODE_DEBUG
				static void debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
			#endif
	};
}