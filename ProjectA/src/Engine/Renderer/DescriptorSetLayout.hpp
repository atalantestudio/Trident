#pragma once

#include "Engine/Renderer/DescriptorSetLayoutBinding.hpp"

namespace ProjectA {
	struct DescriptorSetLayout {
		std::vector<DescriptorSetLayoutBinding> bindings;
	};
}