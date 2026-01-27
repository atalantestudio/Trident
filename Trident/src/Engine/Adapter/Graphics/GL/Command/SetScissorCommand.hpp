// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"

namespace Trident {
	class GL::SetScissorCommand final : public GL::Command {
		public:
			explicit SetScissorCommand(GLint xOffset, GLint yOffset, Extent2D extent) :
				xOffset(xOffset),
				yOffset(yOffset),
				extent(extent)
			{}

			void execute() const final override {
				glScissor(xOffset, yOffset, extent.width, extent.height);
			}

		private:
			GLint xOffset;
			GLint yOffset;
			Extent2D extent;
	};
}