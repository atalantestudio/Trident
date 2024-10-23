#include "pch.hpp"
#include "OpenGLRendererAPI.hpp"
#include <glad/glad.h>

namespace ProjectA {
	void OpenGLRendererAPI::initialize() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::setViewport(std::int32_t x, std::int32_t y, std::uint32_t width, std::uint32_t height) const {
		glViewport(x, y, width, height);
	}

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