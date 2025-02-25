#pragma once

#include "Engine/Renderer/Buffer/IndexBuffer.hpp"

namespace ProjectA {
	class OpenGLIndexBuffer : public IndexBuffer {
		private:
			uint32 count;

		public:
			OpenGLIndexBuffer(const uint32* indices, uint32 count);

			~OpenGLIndexBuffer();

			uint32 getCount() const override;

			void bind() const override;

			void unbind() const override;
	};
}