// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/ComputePipeline.hpp"

namespace ProjectA {
	class GL::ComputePipeline final : public ProjectA::ComputePipeline {
		public:
			explicit ComputePipeline(const ComputePipelineDescriptor& descriptor);

			~ComputePipeline() final override;

		public:
			const GLuint program;
	};
}