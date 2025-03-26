#pragma once

#include <glad/glad.h>

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/ShaderModule.hpp"

namespace ProjectA {
	class OpenGLShaderModule : public ShaderModule, public ObjectWrapper<GLuint> {
		public:
			OpenGLShaderModule(const ShaderModuleDescriptor& descriptor, const chain<uint8>& code);

			~OpenGLShaderModule() override;
	};
}