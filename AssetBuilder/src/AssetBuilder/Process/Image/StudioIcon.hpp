// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Image/ImageProcess.hpp"

namespace Trident {
	class StudioIconImage : public ImageProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "studio_icon";
			}

			ImageAssetDescriptor createImage(std::filesystem::path rootInputPath) const final override {
				const std::filesystem::path path = rootInputPath / "Asset/External/studio_icon.png";
				const std::string pathString = path.string();

				ImageAssetDescriptor descriptor = openImage(view<char8>(&pathString[0], pathString.size()));

				DEBUG_ASSERT(descriptor.format == ImageFormat::R8G8B8A8_UNORM);

				sequence<uint8> data(descriptor.extent.x * descriptor.extent.y);

				convert<ImageFormat::R8G8B8A8_UNORM, ImageFormat::R8_UNORM>(descriptor.data, &data[0], data.count());

				descriptor.identifier = AssetLibrary::createAssetIdentifier();
				descriptor.version = {0, 0, 0};
				descriptor.format = ImageFormat::R8_UNORM;
				descriptor.data = data;

				return descriptor;
			}

		public:
			using ImageProcess::ImageProcess;
	};
}