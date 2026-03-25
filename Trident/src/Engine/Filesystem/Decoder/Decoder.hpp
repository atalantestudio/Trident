// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/File.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace Trident {
	template<FileFormat FileFormat>
	class Decoder {
		public:
			static bool validateSignature(const ByteStream<StreamOperation::READ>& stream);

			static File<FileFormat> decode(view<uint8> data);
	};
}