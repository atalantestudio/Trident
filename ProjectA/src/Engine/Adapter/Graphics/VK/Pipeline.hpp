// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Pipeline.hpp"

namespace ProjectA {
	class VK::Pipeline : public ProjectA::Pipeline {
		public:
			explicit Pipeline(VkPipelineBindPoint bindPoint) :
				bindPoint(bindPoint)
			{}

		public:
			VkPipeline pipeline;
			VkPipelineBindPoint bindPoint;
	};
}