#pragma once

#include "Engine/API/Rendering/ImageFormat.hpp"
#include "Engine/Filesystem/FileFormat.hpp"

namespace ProjectA {
	template<FileFormat FileFormat>
	struct File;

	template<>
	struct File<FileFormat::PNG> {
		ImageFormat format;

		// Only a depth of 1 is supported.
		Extent3D extent;

		// Uncompressed and filtered data.
		sequence<uint8> data;
	};

	template<>
	struct File<FileFormat::QOI> {
		ImageFormat format;
		Extent3D extent;

		// Uncompressed and filtered data.
		sequence<uint8> data;
	};
}