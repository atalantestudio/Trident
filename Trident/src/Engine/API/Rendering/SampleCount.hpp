#pragma once

namespace Trident {
	static bool validateSampleCount(uint8 sampleCount) {
		switch (sampleCount) {
			case 1:
			case 2:
			case 4:
			case 8:
			case 16:
			case 32:
			case 64:
				return true;
			default:
				return false;
		}
	}
}