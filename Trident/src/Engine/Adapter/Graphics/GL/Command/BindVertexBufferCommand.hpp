// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Buffer.hpp"
#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"

namespace Trident {
	class GL::BindVertexBufferCommand final : public GL::Command {
		public:
			explicit BindVertexBufferCommand(GLuint vertexArray, GLuint bindingIndex, GLuint buffer, GLintptr offset, GLsizei stride) :
				vertexArray(vertexArray),
				bindingIndex(bindingIndex),
				buffer(buffer),
				offset(offset),
				stride(stride)
			{}

			void execute() const final override {
				#if OPENGL_VERSION >= OPENGL_VERSION_4_3
					glBindVertexArray(vertexArray);
					glBindVertexBuffer(bindingIndex, buffer, offset, stride);
				#else
					ENGINE_ERROR_UNSUPPORTED();
				#endif
			}

		private:
			GLuint vertexArray;
			GLuint bindingIndex;
			GLuint buffer;
			GLintptr offset;
			GLsizei stride;
	};
}