#pragma once

#include "Engine/types.hpp"

namespace ProjectA {
	template<typename T>
	class fchain {
		public:
			fchain(uint64 count) :
				array(new T[count]()),
				count(count)
			{}

			~fchain() {
				delete[] array;
			}

			T operator [](uint64 index) const {
				ENGINE_ASSERT(index < count, "Index overflow.");

				return array[index];
			}

			T& operator [](uint64 index) {
				ENGINE_ASSERT(index < count, "Index overflow.");

				return array[index];
			}

			const T* begin() const {
				return &array[0];
			}

			T* begin() {
				return &array[0];
			}

			const T* end() const {
				return &array[count];
			}

			T* end() {
				return &array[count];
			}

		public:
			const uint64 count;

		private:
			T* array;
	};
}