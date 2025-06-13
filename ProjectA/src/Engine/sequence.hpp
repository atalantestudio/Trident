#pragma once

#include "Engine/types.hpp"

namespace ProjectA {
	// A non-resizeable array of elements.
	template<typename T>
	class sequence {
		public:
			sequence() = default;

			explicit sequence(uint64 count) :
				array(new T[count]()),
				count(count)
			{}

			sequence(const std::initializer_list<T>& list) :
				sequence(list.size())
			{
				std::copy(list.begin(), list.end(), array);
			}

			sequence(sequence&& sequence) :
				array(sequence.array),
				count(sequence.count)
			{
				sequence.array = nullptr;
				sequence.count = 0;
			}

			~sequence() {
				delete[] array;
			}

			operator T*() const {
				return array;
			}

			sequence& operator =(const sequence& sequence) {
				delete[] array;

				array = new T[sequence.count]();
				count = sequence.count;

				std::copy(sequence.begin(), sequence.end(), array);

				return *this;
			}

			sequence& operator =(sequence&& sequence) {
				array = sequence.array;
				count = sequence.count;

				sequence.array = nullptr;
				sequence.count = 0;

				return *this;
			}

			template<typename _T = T>
			std::enable_if_t<(sizeof(T) <= 8), _T> operator [](uint64 index) const {
				DEBUG_ASSERT(index < count);

				return array[index];
			}

			template<typename _T = T>
			std::enable_if_t<(sizeof(T) > 8), const _T&> operator [](uint64 index) const {
				DEBUG_ASSERT(index < count);

				return array[index];
			}

			T& operator [](uint64 index) {
				DEBUG_ASSERT(index < count);

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

		private:
			T* array = nullptr;

		public:
			uint64 count = 0;
	};
}