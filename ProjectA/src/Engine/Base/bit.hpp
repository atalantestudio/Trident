#pragma once

namespace ProjectA {
	// https://web.archive.org/web/20151229003112/http://blogs.msdn.com/b/jeuge/archive/2005/06/08/hakmem-bit-count.aspx
	static uint32 countSetBits(uint32 number) {
		const uint32 setBitCount = number - ((number >> 1) & 033333333333) - ((number >> 2) & 011111111111);

		return ((setBitCount + (setBitCount >> 3)) & 030707070707) % 63;
	}
}