// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DescriptorSetLayout.hpp"
#include "Engine/API/Graphics/ShaderStage.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"

namespace Trident {
	struct ShaderModuleAsset : public Asset {
		// TODO(tsl/shader-reflection): Remove.
		/// Path relative to the renderer being used.
		/// Only used if the asset's version is below 1.0.0.
		sequence<char8> name;

		ShaderStage stage;

		// TODO(tsl/shader-reflection): Replace by an offset.
		/// Only used if the asset's version is below 1.0.0.
		sequence<char8> entryPoint;

		// TODO(filesystem/asset/shader): Add support for multiple descriptor set layouts.
		/// The bindings of the descriptor set layout used by the shader module.
		sequence<DescriptorSetLayoutBinding> descriptorSetLayoutBindings;

		/// Only used if the asset's version is 1.0.0 or later.
		sequence<char8> glslCode;

		/// Only used if the asset's version is 1.0.0 or later.
		std::unordered_map<sequence<char8>, uint32> glslEntryPointActivationOffsets;
	};
}