// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/AssetBuilderLibrary.hpp"
#include "AssetBuilder/API/Utilities/Utilities.hpp"
#include "AssetBuilder/Process/Image/ImageProcess.hpp"

namespace Trident {
	class CopyImageProcess final : public Process {
		public:
			explicit CopyImageProcess(AssetBuilderLibrary* assetLibrary) :
				assetLibrary(assetLibrary)
			{}

			sint32 main(view<void*> arguments) override {
				const std::filesystem::path rootInputPath = static_cast<const char8*>(arguments[1]);
				const std::filesystem::path rootOutputPath = static_cast<const char8*>(arguments[2]);
				const std::filesystem::path outputDirectoryPath = rootOutputPath / "Asset/Image/";
				const std::filesystem::recursive_directory_iterator imageDirectoryIterator(rootInputPath / "Asset/Image/");

				for (const std::filesystem::directory_entry& directoryEntry : imageDirectoryIterator) {
					if (directoryEntry.is_directory()) {
						continue;
					}

					std::filesystem::path outputPath = outputDirectoryPath / directoryEntry.path().filename();
					outputPath.replace_extension("tia");

					if (std::filesystem::exists(outputPath)) {
						ENGINE_INFO("Image asset \"[]\" already exists.", outputPath.filename().string());

						continue;
					}

					const std::filesystem::path directoryEntryPath = directoryEntry.path();
					const std::string path = directoryEntryPath.string();
					const std::string stem = directoryEntryPath.stem().string();
					const view<char8> assetName = std::string_view(stem);

					ImageAssetDescriptor image = openImage(std::string_view(path));
					image.identifier = AssetLibrary::createAssetIdentifier();

					const ByteStream<StreamOperation::WRITE> stream = ImageProcess::encodeImageAsset(image);

					ImageProcess::createImageAsset(image, stream, assetName, outputDirectoryPath, assetLibrary);

					ENGINE_INFO("Copied image asset \"[]\" (ID: []).", assetName, image.identifier);
				}

				ENGINE_INFO("Done building image assets.");

				return 0;
			}

		private:
			AssetBuilderLibrary* assetLibrary;
	};
}