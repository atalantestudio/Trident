#pragma once

#include "Engine/API/Rendering/DescriptorSetLayoutBinding.hpp"

namespace ProjectA {
	struct DescriptorSetLayout {
		std::vector<DescriptorSetLayoutBinding> bindings;
	};
}