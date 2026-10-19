// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Utilities/Utilities.hpp"
#include "AssetBuilder/Process/Image/ImageProcess.hpp"

namespace Trident {
	/// The paths are relative to the Asset/External directory.
	struct ARMMapDescriptor {
		view<char8> aoMapPath;
		view<char8> roughnessMapPath;
	};

	class ARMMapFromAORoughnessProcess : public ImageProcess {
		protected:
			ImageAssetDescriptor createImage(std::filesystem::path rootInputPath) const final override {
				const std::filesystem::path externalDirectoryPath = rootInputPath / "Asset/External";
				const ARMMapDescriptor descriptor = getDescriptor();

				std::filesystem::path path = externalDirectoryPath / std::string_view(descriptor.aoMapPath);
				std::string pathString = path.string();

				const ImageAssetDescriptor aoMapImage = openImage(view<char8>(&pathString[0], pathString.size()));

				path = externalDirectoryPath / std::string_view(descriptor.roughnessMapPath);
				pathString = path.string();

				const ImageAssetDescriptor roughnessMapImage = openImage(view<char8>(&pathString[0], pathString.size()));

				ENGINE_ASSERT(aoMapImage.format == roughnessMapImage.format, "The map formats don't match.");
				ENGINE_ASSERT(aoMapImage.extent == roughnessMapImage.extent, "The map extents don't match.");
				ENGINE_ASSERT(aoMapImage.extent.z == 1, "The map depths must both be 1.");
				ENGINE_ASSERT(aoMapImage.mipLevelCount == 1, "The map mip level counts must both be 1.");
				ENGINE_ASSERT(aoMapImage.layerCount == 1, "The map layer counts must both be 1.");

				const uint8 channelCount = getChannelCount(aoMapImage.format);

				ImageAssetDescriptor armMapImage{};
				armMapImage.identifier = AssetLibrary::createAssetIdentifier();
				armMapImage.version = {0, 0, 0};
				armMapImage.format = aoMapImage.format;
				armMapImage.extent = aoMapImage.extent;
				armMapImage.mipLevelCount = aoMapImage.mipLevelCount;
				armMapImage.layerCount = aoMapImage.layerCount;
				armMapImage.data = sequence<uint8>(getByteSize(aoMapImage.format) * aoMapImage.extent.x * aoMapImage.extent.y);

				uint32 y = 0;

				while (y < aoMapImage.extent.y) {
					uint32 x = 0;

					while (x < aoMapImage.extent.x) {
						const uint8 ao = aoMapImage.data[(y * aoMapImage.extent.x + x) * channelCount];
						const uint8 roughness = roughnessMapImage.data[(y * aoMapImage.extent.x + x) * channelCount];
						const uint8 metallic = 0;

						armMapImage.data[(y * aoMapImage.extent.x + x) * channelCount + 0] = ao;
						armMapImage.data[(y * aoMapImage.extent.x + x) * channelCount + 1] = roughness;
						armMapImage.data[(y * aoMapImage.extent.x + x) * channelCount + 2] = metallic;

						if (armMapImage.format == ImageFormat::R8G8B8A8_UNORM) {
							armMapImage.data[(y * aoMapImage.extent.x + x) * channelCount + 3] = 255;
						}

						x += 1;
					}

					y += 1;
				}

				return armMapImage;
			}

			virtual ARMMapDescriptor getDescriptor() const = 0;

		public:
			using ImageProcess::ImageProcess;
	};
}