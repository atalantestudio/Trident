// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/Fence.hpp"

namespace Trident {
	class GL::Fence final : public Trident::Fence {
		public:
			explicit Fence(const Trident::Device* device);

		public:
			GLsync sync = nullptr;
	};

	namespace GL {
		ref<Trident::Fence> createFence(const Trident::Device* device, const FenceDescriptor& descriptor);

		void destroyFence(const Trident::Device* device, const Trident::Fence* fence);
	}
}