// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Instance.hpp"
#include "Engine/API/Rendering/PhysicalDevice.hpp"

namespace Trident {
	class DX12::PhysicalDevice final : public Trident::PhysicalDevice {
		public:
			explicit PhysicalDevice(const DX12::Instance& instance);

		private:
			void setLimits();

		public:
			IDXGIAdapter* adapter;
	};
}