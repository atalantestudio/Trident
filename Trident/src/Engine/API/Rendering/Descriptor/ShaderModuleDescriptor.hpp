// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/ShaderStage.hpp"

namespace Trident {
	struct ShaderModuleDescriptor {
		ShaderStage stage;
		view<char8> entryPoint;
	};
}