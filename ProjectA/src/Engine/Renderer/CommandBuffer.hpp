#pragma once

namespace ProjectA {
	class CommandBuffer {
		protected:
			CommandBuffer(uint32 commandCount) :
				commandCount(commandCount)
			{}

		public:
			uint32 getCommandCount() const {
				return commandCount;
			}

		private:
			const uint32 commandCount;
	};
}