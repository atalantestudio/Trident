#pragma once

#include "Engine/Filesystem/Asset/FontAsset.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Filesystem/File.hpp"
#include "Engine/Filesystem/OpenMode.hpp"
#include "Engine/Filesystem/Reader.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"
#include "Engine/Filesystem/Writer.hpp"

namespace ProjectA {
	namespace Filesystem {
		// Returns the path to the project's root directory.
		std::filesystem::path getProjectRootDirectory();

		// Returns the name of the current working directory (CWD).
		std::filesystem::path getCurrentDirectory();

		// Returns the data of the file at the provided path as a byte sequence.
		// `path` is relative to the working directory.
		sequence<uint8> openBuffer(const str8& path);

		// TODO
		// Opens a file for permanent reading or writing.
		// `path` is relative to the working directory.
		// The returned value is the file pointer. It must be managed by the application.
		// template<OpenMode OpenMode, typename StreamType>
		// StreamType* open(std::string_view path);

		Reader openReader(const str8& path);

		Writer openWriter(const str8& path);

		template<FileFormat FileFormat>
		bool validateSignature(const ByteStream<StreamOperation::READ>& stream);

		template<FileFormat FileFormat>
		File<FileFormat> decode(const sequence<uint8>& data);

		template<typename AssetType>
		ref<AssetType> decodeAsset(ByteStream<StreamOperation::READ>& byteStream);

		void QOIDecoderContentOnly(ByteStream<StreamOperation::READ>& stream, sequence<uint8>& data, uint8 channelCount, uint64 maxIterationCount);
	};
}