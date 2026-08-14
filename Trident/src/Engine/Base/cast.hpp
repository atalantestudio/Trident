// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	template<typename T, typename... U>
	constexpr ref<T> createRef(U&&... arguments) {
		ref<T> ref = new T(std::forward<U>(arguments)...);
		ref.owning = true;

		return ref;
	}

	template<typename T, typename... U>
	constexpr ref<T> allocateRef(U&&... arguments) {
		ref<T> ref = new T(std::forward<U>(arguments)...);
		ref.owning = true;

		return ref;
	}

	template<typename T, typename... U>
	constexpr scope<T> createScope(U&&... arguments) {
		return std::make_unique<T>(std::forward<U>(arguments)...);
	}
}