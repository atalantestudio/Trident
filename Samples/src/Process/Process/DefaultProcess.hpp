#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class DefaultProcess : public Process {
		public:
			explicit DefaultProcess() {
				BREAKPOINT();
			}
	};
}