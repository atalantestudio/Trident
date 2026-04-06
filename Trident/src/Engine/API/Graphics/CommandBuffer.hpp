// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class CommandBuffer {
		public:
			inline uint32 getCommandCount() const {
				return commandCount;
			}

		protected:
			explicit CommandBuffer(uint32 commandCount) :
				commandCount(commandCount)
			{}

		private:
			uint32 commandCount;
	};
}