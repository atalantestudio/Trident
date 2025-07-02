#pragma once

#define DEFINE_UDL(literal, type) \
	constexpr type operator ""_##literal(unsigned long long int a) { \
		const type b = static_cast<type>(a); \
		\
		DEBUG_ASSERT(a == static_cast<unsigned long long int>(b)); \
		\
		return b; \
	} \
	\
	constexpr type operator ""_##literal(long double a) { \
		return static_cast<type>(a); \
	}

namespace ProjectA {
	DEFINE_UDL(px, float32);
	DEFINE_UDL(kg, float32);
}