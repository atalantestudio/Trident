// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/ImageFormat.hpp"
#include "Engine/Filesystem/File.hpp"

namespace Trident {
	template<>
	struct File<FileFormat::PNG> {
		ImageFormat format;

		// Depths > 1 are not supported.
		Extent3D extent;

		// Uncompressed and filtered data.
		sequence<uint8> data;
	};
}