// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/RenderPass.hpp"
#include "Engine/Layer/Layer.hpp"

namespace Trident {
	class RenderPassLayer : public Layer {
		public:
			ref<RenderPass> renderPass;
	};
}