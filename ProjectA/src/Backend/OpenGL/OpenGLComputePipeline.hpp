#pragma once

#include <glad/glad.h>

#include "Engine/Renderer/ComputePipeline.hpp"

namespace ProjectA {
	class OpenGLComputePipeline : public ComputePipeline {
		public:
			OpenGLComputePipeline(const ComputePipelineDescriptor& descriptor);

			~OpenGLComputePipeline();

		public:
			chain<DescriptorSetLayout> descriptorSetLayouts;

			const GLuint program;
	};
}