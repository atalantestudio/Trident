// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Utilities/Utilities.hpp"
#include "AssetBuilder/Process/Image/ImageProcess.hpp"

namespace Trident {
	class CubeMapProcess : public ImageProcess {
		protected:
			ImageAssetDescriptor createImage(std::filesystem::path rootInputPath) const final override {
				const view<char8> assetName = getAssetName();
				const view<view<char8>> facePaths = getFacePaths();

				ImageAssetDescriptor cubeMapImage{};
				cubeMapImage.identifier = AssetLibrary::createAssetIdentifier();
				cubeMapImage.version = {0, 0, 0};
				cubeMapImage.format = ImageFormat::R8G8B8A8_UNORM;
				cubeMapImage.extent = {0, 0, 0};
				cubeMapImage.mipLevelCount = 1;
				cubeMapImage.layerCount = 6;

				std::vector<uint8> data;
				uint64 faceDataOffset = 0;
				uint32 faceIndex = 0;

				while (faceIndex < 6) {
					const std::filesystem::path path = rootInputPath / "Asset/External" / std::string_view(facePaths[faceIndex]);
					const std::string pathString = path.string();
					const ImageAssetDescriptor faceImage = openImage(view<char8>(&pathString[0], pathString.size()));

					ENGINE_ASSERT(faceImage.format == cubeMapImage.format, "Unsupported cube map face format.");
					ENGINE_ASSERT(faceImage.extent == cubeMapImage.extent || cubeMapImage.extent == Extent3D(0, 0, 0), "Unsupported cube map face depth.");
					ENGINE_ASSERT(faceImage.extent.depth == 1, "Unsupported cube map face depth.");
					ENGINE_ASSERT(faceImage.mipLevelCount == 1, "Unsupported cube map face mip level count.");
					ENGINE_ASSERT(faceImage.layerCount == 1, "Unsupported cube map face layer count.");

					cubeMapImage.format = faceImage.format;
					cubeMapImage.extent = faceImage.extent;

					data.resize(data.size() + faceImage.data.count());

					std::copy(faceImage.data.begin(), faceImage.data.end(), &data[faceDataOffset]);

					faceDataOffset = data.size();
					faceIndex += 1;
				}

				cubeMapImage.data = sequence<uint8>(data.size());

				std::copy(data.begin(), data.end(), &cubeMapImage.data[0]);

				return cubeMapImage;
			}

			/// Returns the 6 faces composing the cube map, in the following order:
			/// 1. Front
			/// 2. Back
			/// 3. Up (might need to be rotated left 3 times)
			/// 4. Down (might need to be rotated left 1 times)
			/// 5. Right
			/// 6. Left
			/// 
			/// The paths are relative to the Asset/External directory.
			/// The up and down faces must be vertically flipped.
			virtual view<view<char8>> getFacePaths() const = 0;

		public:
			using ImageProcess::ImageProcess;
	};
}