// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	/// Number of samples per pixel supported for image storage operations.
	enum class ImageSampleCount : uint8 {
		/// Specifies an image with 1 sample per pixel.
		x1 = 1,

		/// Specifies an image with 2 samples per pixel.
		x2 = 2,

		/// Specifies an image with 4 samples per pixel.
		x4 = 4,

		count,
	};
}