// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Instance.hpp"

namespace Trident {
	class DX12::Instance final : public Trident::Instance {
		public:
			explicit Instance(const Trident::GraphicsAdapter* adapter, const InstanceDescriptor& descriptor);

			~Instance() final override;

			IDXGIAdapter* selectAdapter(bool preferIntegratedGpu) const;

		private:
			bool isIntegratedAdapter(const DXGI_ADAPTER_DESC& adapterDescriptor) const;

			uint32 calculateAdapterScore(const DXGI_ADAPTER_DESC& adapterDescriptor, bool preferIntegratedGpu) const;

		public:
			IDXGIFactory4* factory;
	};
}