#pragma once

#include <glad/glad.h>

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/ShaderModule.hpp"

namespace ProjectA {
	class OpenGLShaderModule : public ShaderModule, public ObjectWrapper<GLuint> {
		public:
			OpenGLShaderModule(const chain<uint8>& code, const ShaderModuleDescriptor& descriptor);

			~OpenGLShaderModule() override;
	};
}