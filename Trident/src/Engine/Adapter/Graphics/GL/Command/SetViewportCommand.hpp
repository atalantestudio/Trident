// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"

namespace Trident {
	class GL::SetViewportCommand final : public GL::Command {
		public:
			explicit SetViewportCommand(GLint x, GLint y, GLsizei width, GLsizei height) :
				x(x),
				y(y),
				width(width),
				height(height)
			{}

			void execute() const final override {
				GLint viewportX = x;
				GLsizei viewportWidth = width;
				GLint viewportY = y;
				GLsizei viewportHeight = height;

				if (width < 0.0f) {
					viewportX += width;
					viewportWidth = -width;
				}

				if (height < 0.0f) {
					viewportY += height;
					viewportHeight = -height;
				}

				glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
			}

		private:
			GLint x;
			GLint y;
			GLsizei width;
			GLsizei height;
	};
}