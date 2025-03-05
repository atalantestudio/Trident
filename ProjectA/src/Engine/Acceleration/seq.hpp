#pragma once

#include "pch.hpp"

#include "Engine/Acceleration/ResizeMode.hpp"

namespace ProjectA {
	template<typename T>
	struct seq {
		public:
			seq(std::size_t count = 2, ResizeMode resizeMode = ResizeMode::SQUARE) :
				_count(count),
				array(new T[_count]),
				resizeFactor(static_cast<std::size_t>(resizeMode))
			{
				ENGINE_ASSERT(count >= 2, "The sequence count cannot be less than 2.");
			}

			~seq() {
				delete[] array;
			}

			// Returns the number of elements in the sequence.
			std::size_t count() const {
				return _count;
			}

			// Returns the size in bytes of the sequence.
			std::size_t size() const {
				return sizeof(T) * _count;
			}

			// Returns a reference to the element at the provided index.
			// The index must not be overflowing.
			const T& operator [](std::size_t index) const {
				ENGINE_ASSERT(index < _count, "Index overflow in the sequence.");

				return array[index];
			}

			// Returns a reference to the element at the provided index.
			// If the index is overflowing, the sequence is resized.
			// After the resize, the index must not be overflowing.
			T& operator [](std::size_t index) {
				if (index >= _count) {
					resize();
				}

				ENGINE_ASSERT(index < _count, "Index overflow in the sequence.");

				return array[index];
			}

		private:
			void resize() {
				const std::size_t resizedCount = _count * (_count * resizeFactor + 2 * (1 - resizeFactor));
				T* resizedArray = new T[resizedCount];

				std::memcpy(resizedArray, array, sizeof(T) * _count);

				delete[] array;

				_count = resizedCount;
				array = resizedArray;

				ENGINE_DEBUG("Resized sequence to store [] elements.", resizedCount);
			}

		private:
			std::size_t _count;
			T* array;
			const std::size_t resizeFactor;
	};
}