#pragma once

namespace ProjectA {
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