#pragma once

#include "Engine/Renderer/Pipeline.hpp"

namespace ProjectA {
	class RenderPipeline : public Pipeline {
		public:
			virtual ~RenderPipeline() = default;
	};
}