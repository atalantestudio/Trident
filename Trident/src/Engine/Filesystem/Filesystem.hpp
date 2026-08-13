// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/Asset/FontAsset.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Filesystem/File.hpp"
#include "Engine/Filesystem/OpenMode.hpp"
#include "Engine/Filesystem/Reader.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"
#include "Engine/Filesystem/Writer.hpp"

namespace Trident {
	namespace Filesystem {
		// Returns the path to the project's root directory.
		std::filesystem::path getProjectRootDirectory();

		// Returns the name of the current working directory (CWD).
		std::filesystem::path getCurrentDirectory();

		// Returns the data of the file at the provided path as a byte sequence.
		// `path` is relative to the working directory.
		sequence<uint8> openByteBuffer(view<char8> path);

		// Returns the data of the file at the provided path as a byte sequence.
		// `path` is relative to the working directory.
		sequence<char8> openTextBuffer(view<char8> path);

		// TODO
		// Opens a file for permanent reading or writing.
		// `path` is relative to the working directory.
		// The returned value is the file pointer. It must be managed by the application.
		// template<OpenMode OpenMode, typename StreamType>
		// StreamType* open(view<char8> path);

		Reader openReader(view<char8> path);

		Writer openWriter(view<char8> path);

		template<typename AssetType>
		AssetType decodeAsset(ByteStream<StreamOperation::READ>& byteStream, AssetLibrary* assetLibrary);

		void decodeQoiContent(ByteStream<StreamOperation::READ>& stream, sequence<uint8>& data, uint8 channelCount, uint32 footerByteSize);
	};
}