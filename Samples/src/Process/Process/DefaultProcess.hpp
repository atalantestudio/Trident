#pragma once

#include "Trident.hpp"

namespace Trident {
	class DefaultProcess : public Process {
		public:
			sint32 main(const sequence<void*>& arguments) override {
				return 0;
			}
	};
}