// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/API/Rendering/Semaphore.hpp"

namespace ProjectA {
	class VK::Semaphore final : public ProjectA::Semaphore {
		public:
			explicit Semaphore(const VK::Device& device);

			~Semaphore() final override;

		public:
			VkSemaphore semaphore;

		private:
			const VK::Device& device;
	};
}