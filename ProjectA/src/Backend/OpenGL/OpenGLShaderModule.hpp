#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/ShaderModule.hpp"

namespace ProjectA {
	class OpenGLShaderModule : public ShaderModule, public ObjectWrapper<GLuint> {
		public:
			// Creates an OpenGL shader from a source code.
			// The entry point must be named "main".
			OpenGLShaderModule(const chain<uint8>& code, ShaderStage stage);

			// Creates an OpenGL shader from a SPIR-V binary.
			// Requires OpenGL >= 4.6.
			OpenGLShaderModule(const chain<uint8>& code, const ShaderModuleDescriptor& descriptor);

			~OpenGLShaderModule() override;

		private:
			void checkCompileStatus() const;
	};
}