#pragma once

#include "Engine/Renderer/DescriptorSetLayoutBinding.hpp"

namespace ProjectA {
	struct DescriptorSetLayout {
		chain<DescriptorSetLayoutBinding> bindings;
	};
}