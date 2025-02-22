#pragma once

namespace ProjectA {
	struct FramebufferDescriptor {
		vec2<float32> size;
		uint32 sampleCount;
		bool renderToSwapchain;
	};

	class Framebuffer {
		public:
			FramebufferDescriptor descriptor;

		public:
			static ref<Framebuffer> create(const FramebufferDescriptor& descriptor);

			Framebuffer(const FramebufferDescriptor& descriptor);

			virtual ~Framebuffer() = default;

			virtual uint32 getColorAttachmentId() const = 0;
			virtual uint32 getDepthAttachmentId() const = 0;

			// TODO: Remove
			virtual void bind() = 0;
			virtual void unbind() = 0;

			// TODO: Use vec2<int32>
			virtual void resize(const vec2<float32>& size) = 0;
	};
}