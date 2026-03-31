// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/API/Process/Process.hpp"

#include "../Subprocess/Encode2DTestScene.hpp"

namespace Trident {
	class BuildSceneAssetProcess : public Process {
		private:
			static void encodeSceneAsset(uint32 identifier, const ByteStream<StreamOperation::WRITE>& stream, view<char8> assetName, const std::filesystem::path& assetOutputPath) {
				const uint64 size = stream.getByteOffset();
				const std::string assetOutputPathString = assetOutputPath.string();

				sequence<char8> outputPath(assetOutputPathString.size() + assetName.count() + 5);
				std::copy(assetOutputPathString.begin(), assetOutputPathString.end(), &outputPath[0]);
				std::copy(assetName.begin(), assetName.end(), &outputPath[assetOutputPathString.size()]);
				std::copy_n(".tsa", 4, &outputPath[assetOutputPathString.size() + assetName.count()]);

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(stream.data, size);

				ENGINE_INFO("Generated scene asset \"[]\" (ID: []).", outputPath, identifier);
			}

		public:
			sint32 main(view<void*> arguments) override {
				const std::filesystem::path rootOutputPath = pointer_cast<const char8*>(arguments[2]);
				const std::filesystem::path assetOutputPath = rootOutputPath / "Asset/Scene/";

				// 2D test scene
				{
					const uint32 identifier = AssetLibrary::createAssetIdentifier();
					const ByteStream<StreamOperation::WRITE> stream = encode2DTestScene(identifier);

					encodeSceneAsset(identifier, stream, "Scene2D", assetOutputPath);
				}

				ENGINE_INFO("Done building scene assets.");

				return 0;
			}
	};
}