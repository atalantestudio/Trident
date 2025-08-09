#pragma once

#include "Engine/Filesystem/Asset/Asset.hpp"

namespace ProjectA {
	struct FontAssetDescriptor : public AssetDescriptor {};

	class FontAsset : public Asset {
		public:
			explicit FontAsset(const FontAssetDescriptor& descriptor) :
				Asset(descriptor)
			{}
	};
}