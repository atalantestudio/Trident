#pragma once

#include "Engine/API/Rendering/Pipeline.hpp"

namespace ProjectA {
	class RenderPipeline : public Pipeline {
		public:
			virtual ~RenderPipeline() = default;
	};
}