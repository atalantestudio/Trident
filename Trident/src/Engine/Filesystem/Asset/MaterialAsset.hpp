// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DescriptorType.hpp"
#include "Engine/API/Graphics/ShaderStage.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"

namespace Trident {
	struct MaterialDescriptorImage {
		ref<ImageAsset> imageAsset;
		uint32 mipLevelOffset;
		uint32 mipLevelCount;
		uint32 layerOffset;
		uint32 layerCount;
	};

	class MaterialAsset : public Asset {
		public:
			AssetDescriptor header;

			// TODO(tsl/shader-reflection): Replace by an asset identifier.
			/// Path relative to the renderer being used.
			sequence<char8> shaderModuleName;

			// TODO(tsl/shader-reflection): Remove.
			ShaderStage stage;

			// TODO(tsl/shader-reflection): Remove.
			sequence<char8> entryPoint;

			// TODO(tsl/shader-reflection): Store this in the shader module asset instead.
			/// The descriptor types used by the shader module.
			sequence<DescriptorType> descriptorBindingTypes;

			sequence<MaterialDescriptorImage> descriptorImages;
	};
}