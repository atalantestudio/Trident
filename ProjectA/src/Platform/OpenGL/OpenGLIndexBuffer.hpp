#pragma once

#include "ProjectA/Renderer/IndexBuffer.hpp"

namespace ProjectA {
	class OpenGLIndexBuffer : public IndexBuffer {
		private:
			uint32_t count;

		public:
			OpenGLIndexBuffer(uint32_t* indices, uint32_t count);

			virtual ~OpenGLIndexBuffer();

			virtual uint32_t getCount() const override;

			virtual void bind() const override;

			virtual void unbind() const override;
	};
}