#pragma once

#include "Backend/OpenGL/OpenGLPipelineLayout.hpp"

#include "Engine/API/Rendering/ComputePipeline.hpp"
#include "Engine/API/Rendering/Descriptor/ComputePipelineDescriptor.hpp"

namespace ProjectA {
	class OpenGLComputePipeline : public ComputePipeline {
		public:
			OpenGLComputePipeline(const ComputePipelineDescriptor& descriptor);

			~OpenGLComputePipeline();

		public:
			const OpenGLPipelineLayout& layout;

			const GLuint program;
	};
}