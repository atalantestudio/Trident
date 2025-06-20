#pragma once

namespace ProjectA {
	class CommandBuffer {
		public:
			uint32 getCommandCount() const {
				return commandCount;
			}

		protected:
			CommandBuffer(uint32 commandCount) :
				commandCount(commandCount)
			{}

		private:
			const uint32 commandCount;
	};
}