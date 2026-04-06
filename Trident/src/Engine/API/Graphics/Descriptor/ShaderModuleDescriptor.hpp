// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/ShaderStage.hpp"

namespace Trident {
	struct ShaderModuleDescriptor {
		ShaderStage stage;
		view<char8> entryPoint;

		#if defined(BUILD_MODE_DEBUG)
			view<char8> name;
		#endif
	};
}