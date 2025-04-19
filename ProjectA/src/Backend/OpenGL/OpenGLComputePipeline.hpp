#pragma once

#include "Backend/OpenGL/OpenGLPipelineLayout.hpp"

#include "Engine/Renderer/ComputePipeline.hpp"

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