#pragma once

#include <glad/glad.h>

#include "Engine/ObjectWrapper.hpp"
#include "Engine/Renderer/ShaderModule.hpp"
#include "Engine/Renderer/ShaderStage.hpp"

namespace ProjectA {
	class OpenGLShaderModule : public ShaderModule, public ObjectWrapper<GLuint> {
		public:
			OpenGLShaderModule(const ShaderStage stage, const std::string& code);

			~OpenGLShaderModule() override;

		private:
			static constexpr GLuint shaderStages[2] = {
				GL_VERTEX_SHADER,
				GL_FRAGMENT_SHADER,
			};
	};
}