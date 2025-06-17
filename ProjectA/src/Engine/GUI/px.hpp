#pragma once

namespace ProjectA {
	#pragma warning(disable: 4455)

	// User-defined literals not prefixed with an underscore are forbidden,
	// but for now I'll leave this here to keep consistency with a future transpiled scripting language.
	constexpr uint32 operator"" px(uint64 a) {
		const uint32 b = static_cast<uint32>(a);

		DEBUG_ASSERT(a == static_cast<uint64>(b));

		return b;
	}
}