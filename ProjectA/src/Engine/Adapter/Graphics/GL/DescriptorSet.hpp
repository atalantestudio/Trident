#pragma once

#include "Engine/API/Rendering/DescriptorSet.hpp"
#include "Engine/API/Rendering/DescriptorType.hpp"

namespace ProjectA {
	class OpenGLDescriptorSet final : public DescriptorSet {
		public:
			struct DescriptorSetBinding {
				DescriptorType descriptorType;
				sequence<DescriptorBuffer> descriptorBuffers;
				sequence<DescriptorImage> descriptorImages;
			};

		public:
			sequence<DescriptorSetBinding> bindings;
	};
}