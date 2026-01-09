// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"
#include "Engine/Adapter/Graphics/GL/ComputePipeline.hpp"

namespace ProjectA {
	class GL::BindComputePipelineCommand final : public GL::Command {
		public:
			explicit BindComputePipelineCommand(ref<GL::ComputePipeline> computePipeline) :
				program(computePipeline->program)
			{}

			void execute() const final override {
				glUseProgram(program);
			}

		private:
			GLuint program;
	};
}