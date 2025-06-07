#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Descriptor/ShaderModuleDescriptor.hpp"

namespace ProjectA {
	class OpenGLShaderModule : public ShaderModule, public ObjectWrapper<GLuint> {
		public:
			// Creates an OpenGL shader from source code.
			// The entry point must be named "main".
			OpenGLShaderModule(const sequence<uint8>& code, ShaderStage stage);

			// Creates an OpenGL shader from SPIR-V binary.
			// Requires OpenGL >= 4.6.
			OpenGLShaderModule(const sequence<uint8>& code, const ShaderModuleDescriptor& descriptor);

			~OpenGLShaderModule() override;

		private:
			void checkCompileStatus() const;
	};
}