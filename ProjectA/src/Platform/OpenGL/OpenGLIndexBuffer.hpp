#pragma once

#include "Engine/Renderer/IndexBuffer.hpp"

namespace ProjectA {
	class OpenGLIndexBuffer : public IndexBuffer {
		private:
			uint32_t count;

		public:
			OpenGLIndexBuffer(const uint32* indices, uint32 count);

			virtual ~OpenGLIndexBuffer();

			virtual uint32_t getCount() const override;

			virtual void bind() const override;

			virtual void unbind() const override;
	};
}