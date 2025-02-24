#pragma once

#include "Engine/Renderer/ShaderStage.hpp"

namespace ProjectA {
	class ShaderModule {
		public:
			static ref<ShaderModule> create(const ShaderStage stage, const std::string& code);

			virtual ~ShaderModule() = default;
	};
}