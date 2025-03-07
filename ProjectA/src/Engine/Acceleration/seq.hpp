#pragma once

#include "pch.hpp"

#include "Engine/Acceleration/ResizeMode.hpp"

namespace ProjectA {
	// TODO: Fix.
	template<typename T>
	struct seq {
		public:
			seq(std::size_t count = 2, ResizeMode resizeMode = ResizeMode::SQUARE) :
				totalCount(count),
				usedCount(0),
				resizeFactor(static_cast<std::size_t>(resizeMode)),
				array(new T[totalCount])
			{
				ENGINE_ASSERT(totalCount >= 2, "The sequence count cannot be less than 2.");
			}

			~seq() {
				delete[] array;
			}

			// Returns the number of alive elements in the sequence.
			std::size_t count() const noexcept {
				return usedCount;
			}

			// Returns the size in bytes of the sequence occupied by alive elements.
			std::size_t size() const noexcept {
				return sizeof(T) * usedCount;
			}

			// Returns a reference to the element at the provided index.
			// The index must not be overflowing the true size of the sequence.
			const T& operator [](std::size_t index) const {
				ENGINE_ASSERT(index < totalCount, "Index overflow in the sequence.");

				return array[index];
			}

			// Returns a reference to the element at the provided index.
			// A resize will happen if the index is overflowing the true size of the sequence.
			// After the resize, the index must not be overflowing.
			T& operator [](std::size_t index) {
				if (index >= totalCount) {
					resize();
				}

				ENGINE_ASSERT(index < totalCount, "Index overflow in the sequence.");

				if (index >= usedCount) {
					usedCount = index + 1;
				}

				return array[index];
			}

			const T* begin() const {
				return &array[0];
			}

			T* begin() {
				return &array[0];
			}

			const T* end() const {
				return &array[usedCount];
			}

			T* end() {
				return &array[usedCount];
			}

		private:
			// Resizes the sequence according to its resize mode.
			void resize() noexcept {
				const std::size_t resizedCount = totalCount * (totalCount * resizeFactor + 2 * (1 - resizeFactor));
				T* resizedArray = new T[resizedCount];

				std::memcpy(resizedArray, array, size());

				delete[] array;

				totalCount = resizedCount;
				array = resizedArray;
			}

		private:
			std::size_t totalCount;
			std::size_t usedCount;
			std::size_t resizeFactor;
			T* array;
	};
}