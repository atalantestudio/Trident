// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"

namespace Trident {
	class GL::DispatchIndirectCommand final : public GL::Command {
		public:
			explicit DispatchIndirectCommand(GLuint buffer, GLintptr indirect) :
				buffer(buffer),
				indirect(indirect)
			{}

			void execute() const final override {
				glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, buffer);

				#if OPENGL_VERSION >= OPENGL_VERSION_4_3
					glDispatchComputeIndirect(indirect);
				#else
					ENGINE_ERROR_UNSUPPORTED();
				#endif
			}

		private:
			GLuint buffer;
			GLintptr indirect;
	};
}