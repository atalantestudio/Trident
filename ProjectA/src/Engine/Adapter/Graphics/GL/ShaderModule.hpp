// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/ShaderModule.hpp"
#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"

namespace ProjectA {
	class GL::ShaderModule final : public ProjectA::ShaderModule {
		public:
			explicit ShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset);

			// TODO: Deprecate.
			explicit ShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code);

			~ShaderModule() final override;

		private:
			void checkCompileStatus() const;

		public:
			GLuint shader;
	};
}