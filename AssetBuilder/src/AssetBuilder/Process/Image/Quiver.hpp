// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Image/ImageProcess.hpp"

namespace Trident {
	class QuiverImage final : public ImageProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "Quiver";
			}

			virtual ImageAssetDescriptor createImage(std::filesystem::path rootInputPath) const final override {
				const ImageAssetDescriptor image = openImage("../Trident/src/Asset/Image/Quiver.png");

				DEBUG_ASSERT(image.format == ImageFormat::R8G8B8A8_UNORM);

				ImageAssetDescriptor descriptor{};
				descriptor.identifier = AssetLibrary::createAssetIdentifier();
				descriptor.version = {0, 0, 0};
				descriptor.format = ImageFormat::R8_UNORM;
				descriptor.extent = image.extent;
				descriptor.mipLevelCount = 1;
				descriptor.layerCount = 1;
				descriptor.data = sequence<uint8>(image.extent.width * image.extent.height);

				convert<ImageFormat::R8G8B8A8_UNORM, ImageFormat::R8_UNORM>(image.data, &descriptor.data[0], descriptor.data.count());

				return descriptor;
			}

		public:
			using ImageProcess::ImageProcess;
	};
}