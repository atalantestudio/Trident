#pragma once

#include "Engine/API/Rendering/PipelineLayout.hpp"

namespace ProjectA {
	class OpenGLPipelineLayout : public PipelineLayout {
		public:
			OpenGLPipelineLayout(const PipelineLayoutDescriptor& descriptor);

		public:
			std::vector<DescriptorSetLayout> descriptorSetLayouts;
	};
}