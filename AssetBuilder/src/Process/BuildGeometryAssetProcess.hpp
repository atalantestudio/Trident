// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Process/Process.hpp"

#include "AssetBuilder/API/AssetBuilderLibrary.hpp"
#include "AssetBuilder/API/Utilities/Utilities.hpp"
#include "AssetBuilder/Process/Geometry/Geometry3DProcess.hpp"

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

					std::filesystem::path outputPath = assetOutputPath / directoryEntry.path().filename();
					outputPath.replace_extension("tga");

					if (std::filesystem::exists(outputPath)) {
						continue;
					}

					if (!Decoder<FileFormat::FBX>::validateSignature(ByteStream<StreamOperation::READ>(&inputData[0], inputData.count(), Endianness::LITTLE_ENDIAN))) {
						continue;
					}

					const uint32 identifier = AssetLibrary::createAssetIdentifier();
					const std::string assetName = directoryEntry.path().stem().string();

					BuildGeometry3D geometry = decodeFBXGeometry(inputData, identifier);

					generateTangents(geometry);

					const ByteStream<StreamOperation::WRITE> stream = Geometry3DProcess::encodeGeometryAsset(identifier, geometry);

					Geometry3DProcess::createGeometryAsset(identifier, stream, view<char8>(&assetName[0], assetName.size()), assetOutputPath, assetLibrary);

					ENGINE_INFO("Generated geometry asset \"[]\" (ID: []).", assetName, identifier);
				}

				return 0;
			}

		private:
			AssetBuilderLibrary* assetLibrary;
	};
}