// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/AssetBuilderLibrary.hpp"

#include "Engine/API/Process/Process.hpp"
#include "Engine/Filesystem/Asset/MaterialAsset.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	class MaterialShaderProcess : public Process {
		private:
			static ByteStream<StreamOperation::WRITE> encodeMaterialAsset(const MaterialAsset& asset) {
				const uint64 size = (
					sizeof(AssetHeader) +
					sizeof(uint32) * 2 +
					asset.descriptorImageAssetIdentifiers.count() * sizeof(uint32)
				);

				ByteStream<StreamOperation::WRITE> stream(size, Endianness::LITTLE_ENDIAN);
				stream.write<AssetSignature>(AssetSignature::MATERIAL);
				stream.write<uint32>(asset.identifier);
				stream.write<VersionNumber>(asset.version);
				stream.write<uint32>(asset.shaderModuleAssetIdentifier);
				stream.write<uint32>(static_cast<uint32>(asset.descriptorImageAssetIdentifiers.count()));

				for (const uint32 descriptorImageAssetIdentifier : asset.descriptorImageAssetIdentifiers) {
					stream.write<uint32>(descriptorImageAssetIdentifier);
				}

				return stream;
			}

			void createMaterialAsset(uint32 identifier, const ByteStream<StreamOperation::WRITE>& stream, view<char8> assetName, const std::filesystem::path& assetOutputPath) {
				const std::string assetOutputPathString = assetOutputPath.string();

				sequence<char8> outputPath(assetOutputPathString.size() + assetName.count() + 5);
				std::copy(assetOutputPathString.begin(), assetOutputPathString.end(), &outputPath[0]);
				std::copy(assetName.begin(), assetName.end(), &outputPath[assetOutputPathString.size()]);
				std::copy_n(".tma", 4, &outputPath[assetOutputPathString.size() + assetName.count()]);

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(stream.data, stream.getByteOffset());

				assetLibrary->registerAsset(identifier, assetName);
			}

		protected:
			explicit MaterialShaderProcess(AssetBuilderLibrary* assetLibrary) :
				assetLibrary(assetLibrary)
			{}

			virtual view<char8> getAssetName() const = 0;

			virtual MaterialAsset createMaterialAsset(std::filesystem::path rootOutputPath) const = 0;

		public:
			sint32 main(view<void*> arguments) final override {
				const std::filesystem::path rootOutputPath = static_cast<const char8*>(arguments[2]);
				const std::filesystem::path assetOutputPath = rootOutputPath / "Asset/Material/";
				const view<char8> assetName = getAssetName();

				std::filesystem::path outputPath = assetOutputPath / std::string(assetName);
				outputPath.replace_extension("tma");

				if (std::filesystem::exists(outputPath)) {
					ENGINE_INFO("Material asset \"[]\" already exists.", outputPath.filename().string());

					return 0;
				}

				MaterialAsset materialAsset = createMaterialAsset(rootOutputPath);
				materialAsset.identifier = AssetLibrary::createAssetIdentifier();

				const ByteStream<StreamOperation::WRITE> stream = encodeMaterialAsset(materialAsset);

				createMaterialAsset(materialAsset.identifier, stream, assetName, assetOutputPath);

				ENGINE_INFO("Generated material asset \"[]\" (ID: []).", assetName, materialAsset.identifier);

				return 0;
			}

		private:
			AssetBuilderLibrary* assetLibrary;
	};
}