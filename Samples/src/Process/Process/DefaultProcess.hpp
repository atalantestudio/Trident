#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class DefaultProcess : public Process {
		public:
			sint32 main(const sequence<void*>& arguments) override {
				BREAKPOINT();

				return 0;
			}
	};
}