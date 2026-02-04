// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct MaterialReference {
		uint32 materialIndex;
		sequence<sequence<char8>> descriptorImagePaths;
	};
}