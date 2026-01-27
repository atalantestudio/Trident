#pragma once

namespace Trident {
	class Mutex {
		public:
			Mutex(const std::string& name);

			~Mutex();

			void acquire() const;

			void release() const;

		private:
			void* data;
	};
}