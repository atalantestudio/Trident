// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Instance.hpp"
#include "Engine/API/Rendering/PhysicalDevice.hpp"

namespace Trident {
	class DX12::PhysicalDevice final : public Trident::PhysicalDevice {
		public:
			explicit PhysicalDevice(const Trident::GraphicsAdapter* adapter, IDXGIAdapter* dxgiAdapter);

		private:
			void setLimits();

		public:
			IDXGIAdapter* adapter;
	};
}