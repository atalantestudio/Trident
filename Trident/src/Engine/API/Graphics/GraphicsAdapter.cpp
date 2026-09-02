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

#include "Engine/API/Graphics/Buffer.hpp"
#include "Engine/API/Graphics/BufferView.hpp"
#include "Engine/API/Graphics/CommandBuffer.hpp"
#include "Engine/API/Graphics/CommandEncoder.hpp"
#include "Engine/API/Graphics/CommandPool.hpp"
#include "Engine/API/Graphics/ComputePipeline.hpp"
#include "Engine/API/Graphics/DescriptorSet.hpp"
#include "Engine/API/Graphics/DescriptorSetLayout.hpp"
#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/DispatchIndirectCommand.hpp"
#include "Engine/API/Graphics/DrawIndexedIndirectCommand.hpp"
#include "Engine/API/Graphics/DrawIndirectCommand.hpp"
#include "Engine/API/Graphics/Fence.hpp"
#include "Engine/API/Graphics/Framebuffer.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/GraphicsAPI.hpp"
#include "Engine/API/Graphics/GraphicsPipeline.hpp"
#include "Engine/API/Graphics/Image.hpp"
#include "Engine/API/Graphics/ImageView.hpp"
#include "Engine/API/Graphics/Instance.hpp"
#include "Engine/API/Graphics/MemoryAllocator.hpp"
#include "Engine/API/Graphics/PipelineLayout.hpp"
#include "Engine/API/Graphics/Queue.hpp"
#include "Engine/API/Graphics/RenderPass.hpp"
#include "Engine/API/Graphics/Sampler.hpp"
#include "Engine/API/Graphics/Semaphore.hpp"
#include "Engine/API/Graphics/ShaderModule.hpp"
#include "Engine/API/Graphics/Status.hpp"
#include "Engine/API/Graphics/Swapchain.hpp"

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