// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class CommandBuffer {
		public:
			uint32 getCommandCount() const;

		protected:
			explicit CommandBuffer(uint32 commandCount);

		private:
			uint32 commandCount;
	};
}