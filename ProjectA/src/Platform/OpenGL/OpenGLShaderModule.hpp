#pragma once

#include <glad/glad.h>

#include "Engine/ObjectWrapper.hpp"
#include "Engine/Renderer/ShaderModule.hpp"
#include "Engine/Renderer/ShaderStage.hpp"

namespace ProjectA {
	class OpenGLShaderModule : public ObjectWrapper<GLuint>, public ShaderModule {
		public:
			// Since there is no virtual constructor, each subclass of ShaderModule will have its own.
			// Thus the constructors should be as close as possible to the actual API that's being used
			// (e.g. for OpenGL: stage as const GLenum and code as const char* const).
			OpenGLShaderModule(const ShaderStage stage, const std::string& code);

			~OpenGLShaderModule() override;

		private:
			static constexpr GLuint shaderStages[2] = {
				GL_VERTEX_SHADER,
				GL_FRAGMENT_SHADER,
			};
	};
}