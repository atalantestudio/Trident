// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/Asset.hpp"

namespace Trident {
	struct ShaderModuleAssetDescriptor : public AssetDescriptor {
		sequence<char8> glslCode;
		std::unordered_map<sequence<char8>, uint32> glslEntryPointActivationOffsets;
	};

	// TODO: ShaderModuleAssetHeader structure.

	class ShaderModuleAsset : public Asset {
		public:
			explicit ShaderModuleAsset(const ShaderModuleAssetDescriptor& descriptor) :
				Asset(descriptor),
				glslCode(descriptor.glslCode),
				glslEntryPointActivationOffsets(descriptor.glslEntryPointActivationOffsets)
			{}

			const sequence<char8>& getGlslCode() const {
				return glslCode;
			}

			const std::unordered_map<sequence<char8>, uint32>& getGlslEntryPointActivationOffsets() const {
				return glslEntryPointActivationOffsets;
			}

		private:
			sequence<char8> glslCode;
			std::unordered_map<sequence<char8>, uint32> glslEntryPointActivationOffsets;
	};
}