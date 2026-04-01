// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"

namespace Trident {
	class GL::SetViewportCommand final : public GL::Command {
		public:
			explicit SetViewportCommand(vec2<float32> origin, vec2<float32> size, float32 minDepth, float32 maxDepth) :
				origin(origin),
				size(size),
				minDepth(minDepth),
				maxDepth(maxDepth)
			{}

			void execute() const final override {
				GLint x = static_cast<GLint>(origin[0]);
				GLint y = static_cast<GLint>(origin[1]);
				GLsizei width = static_cast<GLsizei>(size[0]);
				GLsizei height = static_cast<GLsizei>(size[1]);

				if (width < 0.0f) {
					x += width;
					width = -width;
				}

				if (height < 0.0f) {
					y += height;
					height = -height;
				}

				glViewport(x, y, width, height);
				glDepthRange(minDepth, maxDepth);
			}

		private:
			vec2<float32> origin;
			vec2<float32> size;
			float32 minDepth;
			float32 maxDepth;
	};
}