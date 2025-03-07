#pragma once

#include <glad/glad.h>

#include "Engine/ObjectWrapper.hpp"
#include "Engine/Renderer/ShaderModule.hpp"

namespace ProjectA {
	class OpenGLShaderModule : public ShaderModule, public ObjectWrapper<GLuint> {
		public:
			// The code string must be terminated.
			OpenGLShaderModule(const GLenum stage, const GLchar* const code);

			~OpenGLShaderModule() override;
	};
}