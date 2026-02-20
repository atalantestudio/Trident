// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/PhysicalDevice.hpp"
#include "Engine/API/Rendering/Surface.hpp"
#include "Engine/API/Window/Window.hpp"

namespace Trident {
	class Instance {
		public:
			~Instance() {
				adapter->destroyInstance(this);
			}

			// TODO: Use ref<Window> instead.
			ref<Surface> createSurface(const Window* window) const {
				return adapter->createSurface(this, window);
			}

			ref<PhysicalDevice> selectPhysicalDevice(bool preferIntegratedGpu) const {
				return adapter->selectPhysicalDevice(this, preferIntegratedGpu);
			}

		protected:
			explicit Instance(const GraphicsAdapter* adapter) :
				adapter(adapter)
			{}

		public:
			// TODO: This should be a protected member but selectPhysicalDevice implementation needs to access it.
			const GraphicsAdapter* adapter;
	};
}