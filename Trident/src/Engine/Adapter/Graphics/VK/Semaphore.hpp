// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Semaphore.hpp"

namespace Trident {
	class VK::Semaphore final : public Trident::Semaphore {
		public:
			explicit Semaphore(const Trident::Device* device);

		public:
			VkSemaphore semaphore;
	};

	namespace VK {
		ref<Trident::Semaphore> createSemaphore(const Trident::Device* device);

		void destroySemaphore(const Trident::Device* device, const Trident::Semaphore* semaphore);
	}
}