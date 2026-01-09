// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"

namespace ProjectA {
	class GL::SetImageMemoryBarrierCommand final : public GL::Command {
		public:
			explicit SetImageMemoryBarrierCommand(GLbitfield barrier) :
				barrier(barrier)
			{}

			void execute() const final override {
				#if OPENGL_VERSION >= OPENGL_VERSION_4_2
					glMemoryBarrier(barrier);
				#else
					ENGINE_ERROR_UNSUPPORTED();
				#endif
			}

		private:
			GLbitfield barrier;
	};
}