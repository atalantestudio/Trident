#pragma once

#include "Engine/Renderer/LoadOperation.hpp"
#include "Engine/Renderer/StoreOperation.hpp"

namespace ProjectA {
	struct RenderPassDescriptor {
		LoadOperation loadOperation;
		StoreOperation storeOperation;
	};

	class RenderPass {
		public:
			virtual ~RenderPass() = default;
	};
}