#pragma once

namespace Trident {
	// File formats supported by the engine for decoding, encoding or/and logging.
	enum class FileFormat : uint8 {
		FBX,
		JSON,
		PNG,
		QOI,
		TTF,
	};
}