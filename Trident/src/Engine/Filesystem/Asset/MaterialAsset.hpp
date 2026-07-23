// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/Asset.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"

namespace Trident {
	struct MaterialDescriptorImage {
		ref<ImageAsset> imageAsset;
	};

	struct MaterialAsset : public Asset {
		/// Index of the shader in the scene.
		uint32 shaderIndex;

		/// Offset in the descriptor set.
		uint32 descriptorOffset;

		sequence<MaterialDescriptorImage> descriptorImages;
	};
}