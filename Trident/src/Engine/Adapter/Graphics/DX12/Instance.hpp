// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Instance.hpp"

namespace Trident {
	class DX12::Instance final : public Trident::Instance {
		public:
			explicit Instance(const Trident::GraphicsAdapter* adapter);

		public:
			IDXGIFactory4* factory;
	};

	namespace DX12 {
		ref<Trident::Instance> createInstance(const Trident::GraphicsAdapter* adapter, const InstanceDescriptor& descriptor);

		void destroyInstance(const Trident::Instance* instance);
	}
}