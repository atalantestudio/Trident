#include "pch.hpp"

#include "Platform/OpenGL/OpenGLRendererAPI.hpp"
#include "Platform/OpenGL/OpenGLRenderPipeline.hpp"

namespace ProjectA {
	void OpenGLRendererAPI::initialize() {
		PROFILE_SCOPE(FUNC_SIG);

		#ifdef BUILD_MODE_DEBUG
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

			glDebugMessageCallback(debugMessageCallback, nullptr);

			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		#endif

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::setViewport(int32 x, int32 y, uint32 width, uint32 height) const {
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::setClearColor(const vec4<float32>& color) const {
		glClearColor(color[0], color[1], color[2], color[3]);
	}

	void OpenGLRendererAPI::clear() const {
		PROFILE_SCOPE(FUNC_SIG);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::drawIndexed(const ref<RenderPipeline>& renderPipeline, uint32 indexCount) const {
		PROFILE_SCOPE(FUNC_SIG);

		if (indexCount == 0) {
			indexCount = renderPipeline->getIndexBuffer()->getCount();
		}

		const GLenum primitiveTopology = ((OpenGLRenderPipeline*) renderPipeline.get())->primitiveTopology;

		glDrawElements(primitiveTopology, indexCount, GL_UNSIGNED_INT, nullptr);

		// Unbind texture at index 0.
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	#ifdef BUILD_MODE_DEBUG
		void OpenGLRendererAPI::debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
			switch (severity) {
				case GL_DEBUG_SEVERITY_NOTIFICATION:
					return ENGINE_INFO("GL: [] (source: [], type: [], id: [])", message, source, type, id);
				case GL_DEBUG_SEVERITY_LOW:
					return ENGINE_WARN("GL: [] (source: [], type: [], id: [])", message, source, type, id);
				case GL_DEBUG_SEVERITY_MEDIUM:
					return ENGINE_ERROR("GL: [] (source: [], type: [], id: [])", message, source, type, id);
				case GL_DEBUG_SEVERITY_HIGH:
					return ENGINE_ERROR("GL: [] (source: [], type: [], id: [])", message, source, type, id);
			}
		}
	#endif
}