#pragma once

#include "Engine/API/Rendering/ImageFormat.hpp"
#include "Engine/Filesystem/FileFormat.hpp"

namespace ProjectA {
	template<FileFormat FileFormat>
	struct File;

	// TODO: Replace with ImageAsset?
	template<>
	struct File<FileFormat::PNG> {
		ImageFormat format;

		// Depth of 1 is supported.
		Extent3D extent;

		// Uncompressed and filtered data.
		sequence<uint8> data;
	};

	// TODO: Replace with ImageAsset?
	template<>
	struct File<FileFormat::QOI> {
		ImageFormat format;
		Extent3D extent;

		// Uncompressed and filtered data.
		sequence<uint8> data;
	};
}