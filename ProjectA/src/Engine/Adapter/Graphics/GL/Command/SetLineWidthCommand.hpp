// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"

namespace ProjectA {
	class GL::SetLineWidthCommand final : public GL::Command {
		public:
			explicit SetLineWidthCommand(GLfloat width) :
				width(width)
			{}

			void execute() const final override {
				glLineWidth(width);
			}

		private:
			GLfloat width;
	};
}