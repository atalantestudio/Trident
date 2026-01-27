#pragma once

#define UDL(literal, type) \
	constexpr type operator ""_##literal(unsigned long long int a) { \
		const type b = static_cast<type>(a); \
		\
		ASSERT(a == static_cast<unsigned long long int>(b)); \
		\
		return b; \
	} \
	\
	constexpr type operator ""_##literal(long double a) { \
		return static_cast<type>(a); \
	}

namespace Trident {
	UDL(deg, float32);
	UDL(kg, float32);
	UDL(kg_m2, float32);
	UDL(m, float32);
	UDL(m_s, float32);
	UDL(m_s2, float32);
	UDL(N, float32);
	UDL(Nm, float32);
	UDL(px, float32);
	UDL(rad, float32);

	// Omega (ω)
	UDL(w, float32);
}