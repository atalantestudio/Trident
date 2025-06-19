#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/ShaderModule.hpp"

namespace ProjectA {
	class OpenGLShaderModule : public ShaderModule, public ObjectWrapper<GLuint> {
		public:
			OpenGLShaderModule(const sequence<uint8>& code, const ShaderModuleDescriptor& descriptor);

			~OpenGLShaderModule() override;

		private:
			void checkCompileStatus() const;
	};
}