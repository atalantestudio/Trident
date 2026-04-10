// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#if defined(ENABLE_DX12_ADAPTER)
	#include "Engine/Adapter/Graphics/DX12/GraphicsAdapter.hpp"
#endif

#if defined(ENABLE_GL_ADAPTER)
	#include "Engine/Adapter/Graphics/GL/GraphicsAdapter.hpp"
#endif

#if defined(ENABLE_VK_ADAPTER)
	#include "Engine/Adapter/Graphics/VK/GraphicsAdapter.hpp"
#endif

#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	ConsoleLogger GraphicsAdapter::logger(std::cout, LogLevel::ALL, "");

	GraphicsAdapter::GraphicsAdapter(const ApplicationProperties& properties) :
		api(properties.GraphicsAPI)
	{
		static constexpr BindAdapterImplementation BIND_ADAPTER_IMPLEMENTATIONS[static_cast<index>(GraphicsAPI::count)] = {
			#if defined(ENABLE_DX12_ADAPTER)
				DX12::bindAdapter,
			#else
				nullptr,
			#endif

			#if defined(ENABLE_GL_ADAPTER)
				GL::bindAdapter,
			#else
				nullptr,
			#endif

			#if defined(ENABLE_VK_ADAPTER)
				VK::bindAdapter,
			#else
				nullptr,
			#endif
		};

		DEBUG_ASSERT(properties.GraphicsAPI < GraphicsAPI::count);

		BindAdapterImplementation bindAdapterImplementation = BIND_ADAPTER_IMPLEMENTATIONS[static_cast<index>(properties.GraphicsAPI)];

		ENGINE_ASSERT(bindAdapterImplementation != nullptr, "The graphics adapter has not been enabled for this API.");

		bindAdapterImplementation(this);
	}

	GraphicsAPI GraphicsAdapter::getApi() const {
		return api;
	}

	ref<Instance> GraphicsAdapter::createInstance(const InstanceDescriptor& descriptor) const {
		return createInstanceImplementation(this, descriptor);
	}
}