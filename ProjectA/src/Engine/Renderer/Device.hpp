#pragma once

#include "Engine/Renderer/ShaderModule.hpp"

namespace ProjectA {
	class Device {
		public:
			virtual ~Device() = default;

			virtual ShaderModule createShaderModule() const = 0;
	};
}