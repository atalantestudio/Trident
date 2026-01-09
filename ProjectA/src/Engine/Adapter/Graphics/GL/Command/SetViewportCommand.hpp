// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"

namespace ProjectA {
	class GL::SetViewportCommand final : public GL::Command {
		public:
			explicit SetViewportCommand(GLint x, GLint y, GLsizei width, GLsizei height) :
				x(x),
				y(y),
				width(width),
				height(height)
			{}

			void execute() const final override {
				glViewport(x, y, width, height);
			}

		private:
			GLint x;
			GLint y;
			GLsizei width;
			GLsizei height;
	};
}