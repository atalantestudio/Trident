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
				ImageAssetDescriptor descriptor = openImage("../Trident/src/Asset/Image/Quiver.png");

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