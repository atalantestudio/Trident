// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DescriptorSetLayout.hpp"
#include "Engine/API/Graphics/ShaderStage.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"

namespace Trident {
	struct ShaderModuleAsset : public Asset {
		ShaderModuleAsset() = default;

		// TODO(tsl/shader-reflection): Remove.
		/// Path relative to the renderer being used.
		/// Only used if the asset's version is below 1.0.0.
		sequence<char8> name;

		ShaderStage stageFlags;

		// TODO(tsl/multiple-entry-points): Use an entry point offset map instead.
		/// Only used if the asset's version is below 1.0.0.
		sequence<char8> entryPoint;

		sequence<DescriptorSetLayoutBinding> bindings;

		/// The bindings of the descriptor set layouts.
		sequence<range<uint32, uint32>> descriptorSetBindingRanges;

		/// Only used if the asset's version is 1.0.0 or later.
		sequence<char8> glslCode;

		/// Only used if the asset's version is 1.0.0 or later.
		std::unordered_map<sequence<char8>, uint32> glslEntryPointActivationOffsets;
	};
}