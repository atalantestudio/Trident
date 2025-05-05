#pragma once

namespace ProjectA {
	// TODO: void* any typedef?

	typedef std::int8_t sint8;
	typedef std::int16_t sint16;
	typedef std::int32_t sint32;
	typedef std::int64_t sint64;

	typedef std::uint8_t uint8;
	typedef std::uint16_t uint16;
	typedef std::uint32_t uint32;
	typedef std::uint64_t uint64;

	typedef std::float_t float32;
	typedef std::double_t float64;

	template<std::size_t C, typename T>
	using vec = lyah::vec<C, T>;

	template<typename T>
	using vec2 = lyah::vec2<T>;

	template<typename T>
	using vec3 = lyah::vec3<T>;

	template<typename T>
	using vec4 = lyah::vec4<T>;

	template<std::size_t M, std::size_t N, typename T>
	using mat = lyah::mat<M, N, T>;

	template<typename T>
	using mat2x2 = lyah::mat2x2<T>;

	template<typename T>
	using mat3x3 = lyah::mat3x3<T>;

	template<typename T>
	using mat4x4 = lyah::mat4x4<T>;

	template<typename T>
	using quat = lyah::quat<T>;

	template<typename T>
	using ref = std::shared_ptr<T>;

	template<typename T>
	using chain = std::vector<T>;

	template<typename T, typename... U>
	constexpr ref<T> createRef(U&&... arguments) {
		return std::make_shared<T>(std::forward<U>(arguments)...);
	}

	template<typename T>
	using scope = std::unique_ptr<T>;

	template<typename T, typename... U>
	constexpr scope<T> createScope(U&&... arguments) {
		return std::make_unique<T>(std::forward<U>(arguments)...);
	}
}