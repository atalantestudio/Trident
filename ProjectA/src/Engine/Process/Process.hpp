#pragma once

namespace ProjectA {
	// Generic one-time computations.
	class Process {
		public:
			uint32 getId() const {
				return id;
			}

			void setId(uint32 id) {
				this->id = id;
			}

			// NOTE: Implement to run a process computation.
			virtual sint32 main(const sequence<void*>& arguments) = 0;

		private:
			// Index of this process in the application process list.
			uint32 id;
	};
}