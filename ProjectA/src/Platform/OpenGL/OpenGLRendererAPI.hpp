#pragma once

#include <glad/glad.h>

#include "Engine/Renderer/RendererAPI.hpp"

namespace ProjectA {
	class OpenGLRendererAPI : public RendererAPI {
		public:
			OpenGLRendererAPI();

			virtual void setViewport(int32 x, int32, uint32 width, uint32 height) const override;

			virtual void setClearColor(const vec4<float32>& color) const override;

			virtual void clear() const override;

			virtual void drawIndexed(const ref<RenderPipeline>& renderPipeline, uint32 indexCount) const override;

		private:
			#ifdef BUILD_MODE_DEBUG
				static void debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
			#endif
	};
}