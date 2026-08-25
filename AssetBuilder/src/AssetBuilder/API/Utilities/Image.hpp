// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Decoder/Decoder.hpp"
#include "Engine/Filesystem/File/QOI.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

#include "AssetBuilder/API/Utilities/Utilities.hpp"
#include "../../../File/PNG.hpp"

namespace Trident {
	ImageAssetDescriptor openImage(view<char8> path) {
		const sequence<uint8> data = Filesystem::openByteBuffer(path);

		ImageAssetDescriptor assetDescriptor{};
		assetDescriptor.identifier = 0;
		assetDescriptor.version = {0, 0, 0};
		assetDescriptor.mipLevelCount = 1;
		assetDescriptor.layerCount = 1;

		if (Decoder<FileFormat::PNG>::validateSignature(ByteStream<StreamOperation::READ>(&data[0], data.count(), Endianness::BIG_ENDIAN))) {
			const File<FileFormat::PNG> file = Decoder<FileFormat::PNG>::decode(data);

			assetDescriptor.format = file.format;
			assetDescriptor.extent = file.extent;
			assetDescriptor.data = file.data;
		}
		else if (Decoder<FileFormat::QOI>::validateSignature(ByteStream<StreamOperation::READ>(&data[0], data.count(), Endianness::LITTLE_ENDIAN))) {
			const File<FileFormat::QOI> file = Decoder<FileFormat::QOI>::decode(data);

			assetDescriptor.format = file.format;
			assetDescriptor.extent = file.extent;
			assetDescriptor.data = file.data;
		}
		else {
			ENGINE_ERROR_UNSUPPORTED();
		}

		return assetDescriptor;
	}
}