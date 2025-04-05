#pragma once

#include "Engine/Renderer/PipelineLayout.hpp"

namespace ProjectA {
	class OpenGLPipelineLayout : public PipelineLayout {
		public:
			OpenGLPipelineLayout(const PipelineLayoutDescriptor& descriptor);

			void bind() const;

		public:
			const chain<DescriptorSetLayout> descriptorSetLayouts;
	};
}