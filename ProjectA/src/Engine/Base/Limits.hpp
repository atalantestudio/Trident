#pragma once

namespace ProjectA {
	// Returns the minimum value representable by T.
	template<typename T>
	static constexpr T min() noexcept;

	template<>
	constexpr sint8 min() noexcept {
		return static_cast<sint8>(0x80);
	}

	template<>
	constexpr sint16 min() noexcept {
		return static_cast<sint16>(0x8000);
	}

	template<>
	constexpr sint32 min() noexcept {
		return static_cast<sint32>(0x80000000);
	}

	template<>
	constexpr sint64 min() noexcept {
		return static_cast<sint64>(0x8000000000000000);
	}

	template<>
	constexpr uint8 min() noexcept {
		return static_cast<uint8>(0);
	}

	template<>
	constexpr uint16 min() noexcept {
		return static_cast<uint16>(0);
	}

	template<>
	constexpr uint32 min() noexcept {
		return static_cast<uint32>(0);
	}

	template<>
	constexpr uint64 min() noexcept {
		return static_cast<uint64>(0);
	}

	template<>
	constexpr float32 min() noexcept {
		return -3.402823466e+38f;
	}

	template<>
	constexpr float64 min() noexcept {
		return -1.7976931348623158e+308;
	}

	// Returns the maximum value representable by T.
	template<typename T>
	static constexpr T max() noexcept;

	template<>
	constexpr sint8 max() noexcept {
		return static_cast<sint8>(0x7f);
	}

	template<>
	constexpr sint16 max() noexcept {
		return static_cast<sint16>(0x7fff);
	}

	template<>
	constexpr sint32 max() noexcept {
		return static_cast<sint32>(0x7fffffff);
	}

	template<>
	constexpr sint64 max() noexcept {
		return static_cast<sint64>(0x7fffffffffffffff);
	}

	template<>
	constexpr uint8 max() noexcept {
		return static_cast<uint8>(0xff);
	}

	template<>
	constexpr uint16 max() noexcept {
		return static_cast<uint16>(0xffff);
	}

	template<>
	constexpr uint32 max() noexcept {
		return static_cast<sint32>(0xffffffff);
	}

	template<>
	constexpr uint64 max() noexcept {
		return static_cast<uint64>(0xffffffffffffffff);
	}

	template<>
	constexpr float32 max() noexcept {
		return 3.402823466e+38f;
	}

	template<>
	constexpr float64 max() noexcept {
		return 1.7976931348623158e+308;
	}
}