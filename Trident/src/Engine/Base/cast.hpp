// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	template<typename T, typename... U>
	constexpr ref<T> createRef(U&&... arguments) {
		return std::make_shared<T>(std::forward<U>(arguments)...);
	}

	template<typename T, typename... U>
	constexpr scope<T> createScope(U&&... arguments) {
		return std::make_unique<T>(std::forward<U>(arguments)...);
	}
}