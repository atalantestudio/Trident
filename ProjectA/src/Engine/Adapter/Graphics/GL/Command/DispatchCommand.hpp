// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"

namespace ProjectA {
	class GL::DispatchCommand final : public GL::Command {
		public:
			explicit DispatchCommand(GLuint numGroupsX, GLuint numGroupsY, GLuint numGroupsZ) :
				numGroupsX(numGroupsX),
				numGroupsY(numGroupsY),
				numGroupsZ(numGroupsZ)
			{}

			void execute() const final override {
				#if OPENGL_VERSION >= OPENGL_VERSION_4_3
					glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
				#else
					ENGINE_ERROR_UNSUPPORTED();
				#endif
			}

		private:
			GLuint numGroupsX;
			GLuint numGroupsY;
			GLuint numGroupsZ;
	};
}