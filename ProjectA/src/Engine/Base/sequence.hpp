#pragma once

namespace ProjectA {
	// A non-resizeable array of elements.
	template<typename T>
	class sequence {
		public:
			sequence() = default;

			explicit sequence(uint64 count) :
				pointer(new T[count]()),
				count(count)
			{}

			sequence(const std::initializer_list<T>& list) :
				sequence(list.size())
			{
				std::copy(list.begin(), list.end(), pointer);
			}

			sequence(const sequence& sequence) :
				pointer(new T[sequence.count]()),
				count(sequence.count)
			{
				std::copy(sequence.begin(), sequence.end(), pointer);
			}

			sequence(sequence&& sequence) :
				pointer(sequence.pointer),
				count(sequence.count)
			{
				sequence.pointer = nullptr;
				sequence.count = 0;
			}

			~sequence() {
				delete[] pointer;
			}

			operator T*() const {
				return pointer;
			}

			sequence& operator =(const sequence& sequence) {
				delete[] pointer;

				pointer = new T[sequence.count]();
				count = sequence.count;

				std::copy(sequence.begin(), sequence.end(), pointer);

				return *this;
			}

			sequence& operator =(sequence&& sequence) {
				pointer = sequence.pointer;
				count = sequence.count;

				sequence.pointer = nullptr;
				sequence.count = 0;

				return *this;
			}

			template<typename T>
			T operator [](uint64 index) const {
				DEBUG_ASSERT(index == 0 || index < count);

				return pointer[index];
			}

			T& operator [](uint64 index) {
				DEBUG_ASSERT(index == 0 || index < count);

				return pointer[index];
			}

			const T* begin() const {
				return &pointer[0];
			}

			T* begin() {
				return &pointer[0];
			}

			const T* end() const {
				return &pointer[count];
			}

			T* end() {
				return &pointer[count];
			}

		private:
			T* pointer = nullptr;

		public:
			uint64 count = 0;
	};
}