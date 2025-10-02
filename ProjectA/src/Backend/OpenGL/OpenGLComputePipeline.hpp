#pragma once

#include "Engine/API/Rendering/ComputePipeline.hpp"

namespace ProjectA {
	class OpenGLComputePipeline final : public ComputePipeline {
		public:
			explicit OpenGLComputePipeline(const ComputePipelineDescriptor& descriptor);

			~OpenGLComputePipeline() final override;

		public:
			const GLuint program;
	};
}