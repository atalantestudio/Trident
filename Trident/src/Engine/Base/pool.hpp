// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	/// A data structure for O(1) addition and O(1) deletion.
	template<typename ElementType, index ElementCount>
	class pool {
		public:
			inline index count() const {
				return _count;
			}

			inline const ElementType& operator[](index index) const {
				DEBUG_ASSERT(index < _count);

				return _elements[index];
			}

			inline void add(const ElementType& element) {
				DEBUG_ASSERT(_count < ElementCount);

				_elements[_count] = element;
				_count += 1;
			}

			inline void remove(index index) {
				DEBUG_ASSERT(index < _count);

				_elements[index] = _elements[_count - 1];
				_count -= 1;
			}

		private:
			ElementType _elements[ElementCount] = nullptr;
			index _count = 0;
	};
}