#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Framebuffer.hpp"

namespace ProjectA {
	class VulkanFramebuffer final : public Framebuffer, public ObjectWrapper<VkFramebuffer> {
		public:
			explicit VulkanFramebuffer(VkDevice device, const FramebufferDescriptor& descriptor);

			~VulkanFramebuffer() final override;

		private:
			VkDevice device;
	};
}