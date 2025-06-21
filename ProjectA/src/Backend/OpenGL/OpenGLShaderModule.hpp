#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/ShaderModule.hpp"

namespace ProjectA {
	class OpenGLShaderModule final : public ShaderModule, public ObjectWrapper<GLuint> {
		public:
			explicit OpenGLShaderModule(const sequence<uint8>& code, const ShaderModuleDescriptor& descriptor);

			~OpenGLShaderModule() final override;

		private:
			void checkCompileStatus() const;
	};
}