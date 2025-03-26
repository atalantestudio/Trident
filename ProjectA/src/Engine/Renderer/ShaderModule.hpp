#pragma once

#include "Engine/Renderer/ShaderStage.hpp"

namespace ProjectA {
	struct ShaderModuleDescriptor {
		ShaderStage stage;
		const char* entryPoint;
	};

	class ShaderModule {
		public:
			virtual ~ShaderModule() = default;
	};
}