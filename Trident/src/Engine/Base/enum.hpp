#pragma once

#define AS_FLAG(T) \
	inline constexpr bool operator ==(T a, std::underlying_type<T>::type b) { \
		return a == static_cast<T>(b); \
	} \
	inline constexpr bool operator !=(T a, std::underlying_type<T>::type b) { \
		return a != static_cast<T>(b); \
	} \
	inline constexpr bool operator <(T a, std::underlying_type<T>::type b) { \
		return a < static_cast<T>(b); \
	} \
	inline constexpr bool operator >(T a, std::underlying_type<T>::type b) { \
		return a > static_cast<T>(b); \
	} \
	inline constexpr T operator ~(T a) { \
		return static_cast<T>(~static_cast<std::underlying_type<T>::type>(a)); \
	} \
	\
	inline constexpr bool operator &(T a, T b) { \
		return static_cast<bool>(static_cast<std::underlying_type<T>::type>(a) & static_cast<std::underlying_type<T>::type>(b)); \
	} \
	\
	inline constexpr T operator |(T a, T b) { \
		return static_cast<T>(static_cast<std::underlying_type<T>::type>(a) | static_cast<std::underlying_type<T>::type>(b)); \
	} \
	inline constexpr T& operator |=(T& a, T b) { \
		return a = a | b; \
	} \
	\
	inline constexpr bool operator &(std::underlying_type<T>::type a, T b) { \
		return a & static_cast<std::underlying_type<T>::type>(b); \
	}

#define AS_PRINTABLE(T) \
	inline std::ostream& operator <<(std::ostream& stream, T a) { \
		return stream << static_cast<std::underlying_type<T>::type>(a); \
	}