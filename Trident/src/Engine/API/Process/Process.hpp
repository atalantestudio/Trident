// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	/// Generic one-time computations.
	class Process {
		public:
			virtual ~Process() = default;

			uint32 getId() const {
				return id;
			}

			void setId(uint32 id) {
				this->id = id;
			}

			/// Implement to run a process computation.
			virtual sint32 main(view<void*> arguments) = 0;

		private:
			/// Index in the application process list.
			uint32 id;
	};
}