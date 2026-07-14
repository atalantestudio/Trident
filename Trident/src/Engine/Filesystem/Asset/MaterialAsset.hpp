// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/Asset.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"

namespace Trident {
	struct MaterialDescriptorImage {
		ref<ImageAsset> imageAsset;

		// TODO(filesystem/asset/material): Use when decoding.
		uint32 mipLevelOffset;
		uint32 mipLevelCount;
		uint32 layerOffset;
		uint32 layerCount;
	};

	struct MaterialAsset : public Asset {
		/// Index of the shader in the scene.
		uint32 shaderIndex;

		sequence<MaterialDescriptorImage> descriptorImages;
	};
}