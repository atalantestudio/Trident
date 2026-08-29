// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Buffer.hpp"
#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"

namespace Trident {
	class GL::BindIndexBufferCommand final : public GL::Command {
		public:
			explicit BindIndexBufferCommand(GLuint vertexArray, GLuint indexBuffer) :
				vertexArray(vertexArray),
				indexBuffer(indexBuffer)
			{}

			void execute() const final override {
				#if OPENGL_VERSION >= OPENGL_VERSION_4_5
					glVertexArrayElementBuffer(vertexArray, indexBuffer);
				#else
					glBindVertexArray(vertexArray);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
				#endif
			}

		private:
			GLuint vertexArray;
			GLuint indexBuffer;
	};
}