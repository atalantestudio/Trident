#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/ShaderModule.hpp"
#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"

namespace ProjectA {
	class OpenGLShaderModule final : public ShaderModule, public ObjectWrapper<GLuint> {
		public:
			explicit OpenGLShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset);

			// TODO: Deprecate.
			explicit OpenGLShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code);

			~OpenGLShaderModule() final override;

		private:
			void checkCompileStatus() const;
	};
}