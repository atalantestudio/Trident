// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/RenderPass.hpp"

namespace Trident {
	class GL::RenderPass final : public Trident::RenderPass {
		public:
			explicit RenderPass(const Trident::Device* device, const Trident::RenderPassDescriptor& descriptor);

		public:
			uint32 presentSourceAttachmentIndex = max<uint32>();
			bool multisample = false;
	};

	namespace GL {
		ref<Trident::RenderPass> createRenderPass(const Trident::Device* device, ref<Trident::Surface> surface, const RenderPassDescriptor& descriptor);

		void destroyRenderPass(const Trident::Device* device, const Trident::RenderPass* renderPass);
	}
}