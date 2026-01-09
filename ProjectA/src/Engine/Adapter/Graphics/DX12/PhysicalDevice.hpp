// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Instance.hpp"
#include "Engine/API/Rendering/PhysicalDevice.hpp"

namespace ProjectA {
	class DX12::PhysicalDevice : public ProjectA::PhysicalDevice {
		public:
			explicit PhysicalDevice(const DX12::Instance& instance);

		public:
			IDXGIAdapter* adapter;
	};
}