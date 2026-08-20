// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/ShaderStage.hpp"

namespace Trident {
	struct ShaderModuleDescriptor {
		/// The stage of this shader module.
		ShaderStage stage;

		/// The name of the entry point function.
		view<char8> entryPoint;

		#if defined(DEBUG_OBJECT_NAME)
			view<char8> name;
		#endif
	};
}