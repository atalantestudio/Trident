#pragma once

#ifdef BUILD_MODE_DEBUG
	#include <glad/glad.h>
#endif

#include "Engine/Renderer/RendererAPI.hpp"

namespace ProjectA {
	class OpenGLRendererAPI : public RendererAPI {
		public:
			OpenGLRendererAPI();

			void setViewport(int32 x, int32, uint32 width, uint32 height) const override;

			void setClearColor(const vec4<float32>& color) const override;

			void clear() const override;

			void drawIndexed(const ref<RenderPipeline>& renderPipeline, uint32 indexCount) const override;

		private:
			#ifdef BUILD_MODE_DEBUG
				static void debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
			#endif
	};
}