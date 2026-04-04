// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"

namespace Trident {
	class GL::InsertMemoryBarriersCommand final : public GL::Command {
		public:
			explicit InsertMemoryBarriersCommand(GLbitfield barriers) :
				barriers(barriers)
			{}

			void execute() const final override {
				#if OPENGL_VERSION >= OPENGL_VERSION_4_2
					glMemoryBarrier(barriers);
				#else
					ENGINE_ERROR_UNSUPPORTED();
				#endif
			}

		private:
			GLbitfield barriers;
	};
}