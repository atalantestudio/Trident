// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/Adapter/Graphics/DX12/GraphicsAdapter.hpp"
#include "Engine/Adapter/Graphics/GL/GraphicsAdapter.hpp"
#include "Engine/Adapter/Graphics/VK/GraphicsAdapter.hpp"

namespace Trident {
	ConsoleLogger GraphicsAdapter::logger(std::cout, LogLevel::ALL, "");

	GraphicsAdapter::GraphicsAdapter(const ApplicationProperties& properties) :
		api(properties.GraphicsAPI)
	{
		static constexpr BindAdapterImplementation BIND_ADAPTER_IMPLEMENTATIONS[static_cast<index>(GraphicsAPI::count)] = {
			DX12::bindAdapter,
			GL::bindAdapter,
			VK::bindAdapter,
		};

		DEBUG_ASSERT(properties.GraphicsAPI < GraphicsAPI::count);

		BIND_ADAPTER_IMPLEMENTATIONS[static_cast<index>(properties.GraphicsAPI)](this);
	}

	GraphicsAPI GraphicsAdapter::getApi() const {
		return api;
	}

	ref<Instance> GraphicsAdapter::createInstance(const InstanceDescriptor& descriptor) const {
		return createInstanceImplementation(this, descriptor);
	}
}