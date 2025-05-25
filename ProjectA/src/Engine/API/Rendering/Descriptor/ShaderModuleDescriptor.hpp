#pragma once

#include "Engine/API/Rendering/ShaderStage.hpp"

namespace ProjectA {
	struct ShaderModuleDescriptor {
		ShaderStage stage;
		std::string_view entryPoint;
	};
}