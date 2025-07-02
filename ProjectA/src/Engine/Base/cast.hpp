#pragma once

namespace ProjectA {
	template<typename O, typename I>
	typename std::enable_if<std::is_const<typename std::remove_pointer<O>::type>::value, O>::type pointer_cast(I pointer) {
		return static_cast<O>(static_cast<const void*>(pointer));
	}

	template<typename O, typename I>
	typename std::enable_if<!std::is_const<typename std::remove_pointer<O>::type>::value, O>::type pointer_cast(I pointer) {
		return static_cast<O>(static_cast<void*>(pointer));
	}

	template<typename T, typename... U>
	constexpr ref<T> createRef(U&&... arguments) {
		return std::make_shared<T>(std::forward<U>(arguments)...);
	}

	template<typename T, typename... U>
	constexpr scope<T> createScope(U&&... arguments) {
		return std::make_unique<T>(std::forward<U>(arguments)...);
	}
}