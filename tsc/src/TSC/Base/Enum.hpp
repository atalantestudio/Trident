#pragma once

#ifndef AS_FLAG
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
		inline constexpr bool operator &(T a, T b) { \
			return (static_cast<sint32>(a) & static_cast<sint32>(b)); \
		} \
		inline constexpr T operator |(T a, T b) { \
			return static_cast<T>(static_cast<sint32>(a) | static_cast<sint32>(b)); \
		} \
		inline constexpr T& operator |=(T& a, T b) { \
			return a = a | b; \
		}
#endif