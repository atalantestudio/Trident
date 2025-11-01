#pragma once

namespace ProjectA {
	// https://www.w3.org/TR/WGSL/#roundup
	static uint32 roundUp(uint32 k, uint32 n) {
		if (k == 0) {
			return 0;
		}

		return (static_cast<uint32>(ceil(static_cast<float32>(n) / k))) * k;
	}
}