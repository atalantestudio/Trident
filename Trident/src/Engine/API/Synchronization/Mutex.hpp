// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class Mutex {
		public:
			explicit Mutex(view<char8> name);

			~Mutex();

			void acquire() const;

			void release() const;

		private:
			void* data;
	};
}