#pragma once

namespace ProjectA {
	#pragma warning(disable: 4455)

	constexpr float32 operator ""_px(uint64 a) {
		const float32 b = static_cast<float32>(a);

		DEBUG_ASSERT(a == static_cast<uint64>(b));

		return b;
	}

	constexpr float32 operator ""_px(long double a) {
		return static_cast<float32>(a);
	}
}