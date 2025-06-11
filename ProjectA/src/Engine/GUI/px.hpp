#pragma once

namespace ProjectA {
	#pragma warning(disable: 4455)

	// User-defined literals not prefixed with an underscore are forbidden,
	// but for now I'll leave this here to keep consistency with a future transpiled scripting language.
	constexpr sint16 operator"" px(uint64 a) {
		const sint16 b = static_cast<sint16>(a);
		const uint64 c = static_cast<uint64>(b);

		DEBUG_ASSERT(a == c);

		return b;
	}
}