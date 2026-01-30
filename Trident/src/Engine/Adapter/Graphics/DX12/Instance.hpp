// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Instance.hpp"
#include "Engine/Application/ApplicationProperties.hpp"

namespace Trident {
	class DX12::Instance final : public Trident::Instance {
		public:
			explicit Instance(const ApplicationProperties& properties);

			~Instance() final override;

			IDXGIAdapter* selectAdapter() const;

		private:
			bool isIntegratedAdapter(const DXGI_ADAPTER_DESC& adapterDescriptor) const;

			uint32 calculateAdapterScore(const DXGI_ADAPTER_DESC& adapterDescriptor) const;

		public:
			IDXGIFactory4* factory;

		private:
			bool preferIntegratedGpu;
	};
}