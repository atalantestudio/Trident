#include "pch.hpp"
#include "OpenGLRendererAPI.hpp"
#include <glad/glad.h>

namespace ProjectA {
	void OpenGLRendererAPI::setClearColor(const vec4<float>& color) const {
		glClearColor(color.x, color.y, color.z, color.w);
	}

	void OpenGLRendererAPI::clear() const {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::drawIndexed(const Ref<VertexArray>& vertexArray) const {
		glDrawElements(GL_TRIANGLES, vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
	}
}