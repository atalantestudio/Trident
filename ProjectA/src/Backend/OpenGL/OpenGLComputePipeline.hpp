#pragma once

#include "Backend/OpenGL/OpenGLPipelineLayout.hpp"

#include "Engine/API/Rendering/ComputePipeline.hpp"
#include "Engine/API/Rendering/Descriptor/ComputePipelineDescriptor.hpp"

namespace ProjectA {
	class OpenGLComputePipeline final : public ComputePipeline {
		public:
			explicit OpenGLComputePipeline(const ComputePipelineDescriptor& descriptor);

			~OpenGLComputePipeline() final override;

		public:
			const OpenGLPipelineLayout& layout;
			const GLuint program;
	};
}