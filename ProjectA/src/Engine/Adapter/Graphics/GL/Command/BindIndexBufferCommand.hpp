// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Buffer.hpp"
#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"

namespace ProjectA {
	class GL::BindIndexBufferCommand final : public GL::Command {
		public:
			explicit BindIndexBufferCommand(GLuint vertexArray, GLuint indexBuffer) :
				vertexArray(vertexArray),
				indexBuffer(indexBuffer)
			{}

			void execute() const final override {
				#if OPENGL_VERSION >= OPENGL_VERSION_4_5
					glVertexArrayElementBuffer(vertexArray, indexBuffer);
				#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
					glBindVertexArray(vertexArray);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
				#else
					ENGINE_ERROR_UNSUPPORTED();
				#endif
			}

		private:
			GLuint vertexArray;
			GLuint indexBuffer;
	};
}