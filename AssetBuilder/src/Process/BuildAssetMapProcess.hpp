// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Process/Process.hpp"
#include "Engine/Filesystem/Asset/AssetMap.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	class BuildAssetMapProcess final : public Process {
		private:
			static constexpr uint32 PROJECT_OUTPUT_PATH_ARGUMENT_INDEX = 2;

		public:
			explicit BuildAssetMapProcess(const AssetMap* assetMap) :
				assetMap(assetMap)
			{}

			sint32 main(view<void*> arguments) override {
				const std::filesystem::path rootOutputPath = static_cast<const char8*>(arguments[PROJECT_OUTPUT_PATH_ARGUMENT_INDEX]);
				const std::filesystem::path assetOutputPath = rootOutputPath / "Asset/";
				const std::string projectNameString = rootOutputPath.filename().string();
				const view<char8> projectName(&projectNameString[0], projectNameString.size());
				const std::filesystem::path assetMapOutputPath = (assetOutputPath / projectNameString).replace_extension("tam");

				if (std::filesystem::exists(assetMapOutputPath)) {
					ENGINE_INFO("The asset map already exists for the project \"[]\".", projectName);

					return 0;
				}

				const std::string assetOutputPathString = assetOutputPath.string();

				AssetHeader assetHeader{};
				assetHeader.signature = AssetSignature::ASSET_MAP;
				assetHeader.version = {0, 0, 0};

				uint64 size = sizeof(AssetSignature) + sizeof(uint32);

				for (const auto& asset : assetMap->newAssets) {
					size += sizeof(uint32) * 2 + sizeof(AssetSignature) + asset.second.assetPath.count();
				}

				ByteStream<StreamOperation::WRITE> stream(size, Endianness::LITTLE_ENDIAN);

				stream.write<AssetSignature>(assetHeader.signature);
				stream.write<VersionNumber>(assetHeader.version);

				for (const auto& asset : assetMap->newAssets) {
					stream.write<uint32>(asset.first);
					stream.write<AssetSignature>(asset.second.signature);
					stream.write<uint32>(static_cast<uint32>(asset.second.assetPath.count()));

					std::copy(asset.second.assetPath.begin(), asset.second.assetPath.end(), stream.getBytePosition());

					stream.jump(asset.second.assetPath.count());
				}

				sequence<char8> outputPath(assetOutputPathString.size() + projectName.count() + 5);

				std::copy(assetOutputPathString.begin(), assetOutputPathString.end(), &outputPath[0]);
				std::copy(projectName.begin(), projectName.end(), &outputPath[assetOutputPathString.size()]);
				std::copy_n(".tam", 4, &outputPath[assetOutputPathString.size() + projectName.count()]);

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(stream.data, size);

				ENGINE_INFO("Generated asset map for project \"[]\".", projectName);

				return 0;
			}

		private:
			const AssetMap* assetMap;
	};
}