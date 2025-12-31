#pragma once

#include "Engine/API/Rendering/ShaderStage.hpp"

namespace ProjectA {
	struct ShaderModuleDescriptor {
		ShaderStage stage;
		view<char8> entryPoint;
	};
}