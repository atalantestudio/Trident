// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/AssetBuilderLibrary.hpp"
#include "AssetBuilder/API/Utilities/Utilities.hpp"
#include "AssetBuilder/Process/Geometry/Geometry3DProcess.hpp"

#include "Engine/API/Process/Process.hpp"

namespace Trident {
	class BuildGeometryAssetProcess final : public Process {
		public:
			explicit BuildGeometryAssetProcess(AssetBuilderLibrary* assetLibrary) :
				assetLibrary(assetLibrary)
			{}

			sint32 main(view<void*> arguments) override {
				const std::filesystem::path rootInputPath = static_cast<const char8*>(arguments[1]);
				const std::filesystem::path rootOutputPath = static_cast<const char8*>(arguments[2]);
				const std::filesystem::path assetOutputPath = rootOutputPath / "Asset/Geometry/";
				const std::filesystem::recursive_directory_iterator geometryDirectoryIterator(rootInputPath / "Asset/Geometry");

				for (const std::filesystem::directory_entry& directoryEntry : geometryDirectoryIterator) {
					if (directoryEntry.is_directory()) {
						continue;
					}

					const sequence<char8> path = directoryEntry.path().string();
					const sequence<uint8> inputData = Filesystem::openByteBuffer(path);

					std::filesystem::path finalAssetOutputPath = assetOutputPath / directoryEntry.path().filename();
					finalAssetOutputPath.replace_extension("tga");

					if (std::filesystem::exists(finalAssetOutputPath)) {
						ENGINE_INFO("Geometry asset \"[]\" already exists.", finalAssetOutputPath.filename().string());

						continue;
					}

					if (!Decoder<FileFormat::FBX>::validateSignature(ByteStream<StreamOperation::READ>(&inputData[0], inputData.count(), Endianness::LITTLE_ENDIAN))) {
						ENGINE_INFO("Ignoring non-FBX file \"[]\".", path);

						continue;
					}

					const uint32 identifier = AssetLibrary::createAssetIdentifier();
					const std::string assetName = directoryEntry.path().stem().string();

					BuildGeometry3D geometry = decodeFBXGeometry(inputData, identifier);

					generateTangents(geometry);

					const ByteStream<StreamOperation::WRITE> stream = Geometry3DProcess::encodeGeometryAsset(identifier, geometry);

					createGeometryAsset(identifier, stream, view<char8>(&assetName[0], assetName.size()), assetOutputPath);
				}

				ENGINE_INFO("Generated FBX geometry assets.");

				return 0;
			}

		private:
			void createGeometryAsset(uint32 identifier, const ByteStream<StreamOperation::WRITE>& stream, view<char8> assetName, const std::filesystem::path& assetOutputPath) {
				const std::string assetOutputPathString = assetOutputPath.string();

				sequence<char8> outputPath(assetOutputPathString.size() + assetName.count() + 5);
				std::copy(assetOutputPathString.begin(), assetOutputPathString.end(), &outputPath[0]);
				std::copy(assetName.begin(), assetName.end(), &outputPath[assetOutputPathString.size()]);
				std::copy_n(".tga", 4, &outputPath[assetOutputPathString.size() + assetName.count()]);

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(stream.data, stream.size);

				assetLibrary->registerAsset(identifier, assetName);
			}

		private:
			AssetBuilderLibrary* assetLibrary;
	};
}