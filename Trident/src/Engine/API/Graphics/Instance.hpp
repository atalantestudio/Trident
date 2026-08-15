// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"

namespace Trident {
	class Instance {
		public:
			~Instance();

			// TODO(graphics/instance): Use ref<Window> instead.
			ref<Surface> createSurface(ref<PhysicalDevice> physicalDevice, const Window* window) const;

			ref<PhysicalDevice> selectPhysicalDevice(bool preferIntegratedGpu) const;

		protected:
			explicit Instance(const GraphicsAdapter* adapter);

		public:
			// TODO(graphics/instance): This should be a protected member.
			const GraphicsAdapter* adapter;
	};
}