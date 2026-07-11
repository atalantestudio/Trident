// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/BuildAssetMap.hpp"
#include "../Subprocess/Material.hpp"

#include "Engine/API/Process/Process.hpp"
#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/Asset/MaterialAsset.hpp"

namespace Trident {
	class BuildMaterialAssetProcess final : public Process {
		private:
			typedef MaterialAsset (*CreateMaterialFunction)(AssetLibrary* assetLibrary);

		private:
			static constexpr uint32 MATERIAL_COUNT = 5;

			inline static const view<char8> MATERIAL_NAMES[MATERIAL_COUNT] = {
				"LightmappedFloor",
				"LightmappedWall",
				"LightmappedDoor",
				"LightmappedTopWall",
				"LightmappedCeiling",
			};

			inline static constexpr CreateMaterialFunction MATERIAL_FUNCTIONS[MATERIAL_COUNT] = {
				createLightmappedFloorMaterial,
				createLightmappedWallMaterial,
				createLightmappedDoorMaterial,
				createLightmappedTopWallMaterial,
				createLightmappedCeilingMaterial,
			};

		private:
			static ByteStream<StreamOperation::WRITE> encodeMaterialAsset(const MaterialAsset& asset) {
				const uint64 size = (
					sizeof(AssetHeader) +
					sizeof(uint32) + asset.shaderModuleName.count() +
					sizeof(ShaderStage) +
					sizeof(uint32) + asset.entryPoint.count() +
					sizeof(uint32) + asset.descriptorBindingTypes.count() +
					sizeof(uint32) + sizeof(MaterialDescriptorImage) * asset.descriptorImages.count()
				);

				ByteStream<StreamOperation::WRITE> stream(size, Endianness::LITTLE_ENDIAN);

				stream.write<AssetSignature>(AssetSignature::MATERIAL);
				stream.write<uint32>(asset.header.identifier);
				stream.write<VersionNumber>(asset.header.version);
				stream.write<uint32>(static_cast<uint32>(asset.shaderModuleName.count()));

				std::copy_n(pointer_cast<const uint8*>(&asset.shaderModuleName[0]), sizeof(asset.shaderModuleName.count()), stream.getBytePosition());

				stream.jump(asset.shaderModuleName.count());
				stream.write<ShaderStage>(asset.stage);
				stream.write<uint32>(static_cast<uint32>(asset.entryPoint.count()));

				std::copy_n(pointer_cast<const uint8*>(&asset.entryPoint[0]), sizeof(asset.entryPoint.count()), stream.getBytePosition());

				stream.jump(asset.entryPoint.count());
				stream.write<uint32>(static_cast<uint32>(asset.descriptorBindingTypes.count()));

				std::copy_n(pointer_cast<const uint8*>(&asset.descriptorBindingTypes[0]), sizeof(asset.descriptorBindingTypes.count()), stream.getBytePosition());

				stream.jump(asset.descriptorBindingTypes.count());
				stream.write<uint32>(static_cast<uint32>(asset.descriptorImages.count()));

				for (const MaterialDescriptorImage& materialDescriptorImage : asset.descriptorImages) {
					stream.write<uint32>(materialDescriptorImage.imageAsset->getIdentifier());
					stream.write<uint32>(materialDescriptorImage.mipLevelOffset);
					stream.write<uint32>(materialDescriptorImage.mipLevelCount);
					stream.write<uint32>(materialDescriptorImage.layerOffset);
					stream.write<uint32>(materialDescriptorImage.layerCount);
				}

				return stream;
			}

			void saveMaterialAsset(const MaterialAsset& asset, const ByteStream<StreamOperation::WRITE>& stream, view<char8> assetName, const std::filesystem::path& assetOutputPath) {
				const std::string assetOutputPathString = assetOutputPath.string();

				sequence<char8> outputPath(assetOutputPathString.size() + assetName.count() + 5);
				std::copy(assetOutputPathString.begin(), assetOutputPathString.end(), &outputPath[0]);
				std::copy(assetName.begin(), assetName.end(), &outputPath[assetOutputPathString.size()]);
				std::copy_n(".tma", 4, &outputPath[assetOutputPathString.size() + assetName.count()]);

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(stream.data, stream.size);

				ENGINE_INFO("Generated material asset \"[]\" (ID: []).", outputPath, asset.identifier);

				assetMap->insert(asset.identifier, assetName);
			}

		public:
			explicit BuildMaterialAssetProcess(BuildAssetMap* assetMap) :
				assetMap(assetMap)
			{}

			sint32 main(view<void*> arguments) override {
				const std::filesystem::path rootOutputPath = static_cast<const char8*>(arguments[2]);
				const std::filesystem::path assetOutputPath = rootOutputPath / "Asset/Material/";

				AssetLibrary assetLibrary(rootOutputPath);
				uint32 materialIndex = 0;

				while (materialIndex < MATERIAL_COUNT) {
					const MaterialAsset asset = MATERIAL_FUNCTIONS[materialIndex](&assetLibrary);
					const ByteStream<StreamOperation::WRITE> stream = encodeMaterialAsset(asset);

					saveMaterialAsset(asset, stream, MATERIAL_NAMES[materialIndex], assetOutputPath);

					materialIndex += 1;
				}

				ENGINE_INFO("Done building material assets.");

				return 0;
			}

		private:
			BuildAssetMap* assetMap;
	};
}