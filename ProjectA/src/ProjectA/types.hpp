#pragma once

namespace ProjectA {
	typedef std::byte byte;

	typedef std::int8_t int8;
	typedef std::int16_t int16;
	typedef std::int32_t int32;
	typedef std::int64_t int64;

	typedef std::uint8_t uint8;
	typedef std::uint16_t uint16;
	typedef std::uint32_t uint32;
	typedef std::uint64_t uint64;

	typedef std::float_t float32;
	typedef std::double_t float64;

	// template<std::size_t C, typename T>
	// using vec = lyah::vec<C, T>;

	// template<std::size_t M, std::size_t N, typename T>
	// using mat = lyah::mat<M, N, T>;

	// template<typename T>
	// using quat = lyah::quat<T>;

	template<typename T>
	using ref = std::shared_ptr<T>;

	template<typename T, typename... U>
	constexpr ref<T> createRef(U&&... arguments) {
		return std::make_shared<T>(std::forward<U>(arguments)...);
	}

	// TODO: Deprecate?
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename... U>
	constexpr Scope<T> CreateScope(U&&... arguments) {
		return std::make_unique<T>(std::forward<U>(arguments)...);
	}
}