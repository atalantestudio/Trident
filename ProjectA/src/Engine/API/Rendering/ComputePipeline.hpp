#pragma once

#include "Engine/API/Rendering/Pipeline.hpp"

namespace ProjectA {
	class ComputePipeline : public Pipeline {
		public:
			virtual ~ComputePipeline() = default;
	};
}