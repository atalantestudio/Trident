// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Stream/ByteStream.hpp"
#include "Engine/Filesystem/Stream/StreamOperation.hpp"

#include "File/TTF.hpp"

namespace Trident {
	namespace TrueTypeFont {
		/// See https://learn.microsoft.com/en-us/typography/opentype/spec/gpos.
		void decodeGPOSTable(ByteStream<StreamOperation::READ>& stream, const std::unordered_map<uint32, uint32>& tableOffsetMap);
	}
}