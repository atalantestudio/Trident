#pragma once

#include "Engine/Renderer/LoadOperation.hpp"
#include "Engine/Renderer/StoreOperation.hpp"

namespace ProjectA {
	struct RenderPassDescriptor {
		uint8 sampleCount = 1;
		LoadOperation loadOperation;
		StoreOperation storeOperation;
	};

	class RenderPass {
		public:
			virtual ~RenderPass() = default;
	};
}