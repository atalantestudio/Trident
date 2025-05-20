#pragma once

#include "Engine/Renderer/ShaderStage.hpp"

namespace ProjectA {
	struct ShaderModuleDescriptor {
		ShaderStage stage;
		std::string_view entryPoint;
	};

	class ShaderModule {
		public:
			virtual ~ShaderModule() = default;
	};
}