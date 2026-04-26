// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#define UDL(literal, type) \
	constexpr type operator""_##literal(unsigned long long int a) { \
		const type b = static_cast<type>(a); \
		\
		ASSERT(a == static_cast<unsigned long long int>(b)); \
		\
		return b; \
	} \
	\
	constexpr type operator""_##literal(long double a) { \
		return static_cast<type>(a); \
	}

namespace Trident {
	// Metrics
	UDL(m, float32);
	UDL(m_s, float32);
	UDL(m_s2, float32);

	// Time
	UDL(ms, float32);
	UDL(s, float32);

	// Trigonometry
	UDL(deg, float32);
	UDL(rad, float32);

	// Weight
	UDL(kg, float32);
	UDL(kg_m2, float32);

	// Forces
	UDL(N, float32);
	UDL(Nm, float32);

	// Omega (ω)
	UDL(w, float32);

	// Pixels
	UDL(px, float32);
}