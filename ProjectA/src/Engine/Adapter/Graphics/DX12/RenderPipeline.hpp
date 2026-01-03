// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/RenderPipeline.hpp"

namespace ProjectA {
	class DX12::RenderPipeline : public ProjectA::RenderPipeline {
		public:
			explicit RenderPipeline(const RenderPipelineDescriptor& descriptor, const DX12::Device& device);
	};
}