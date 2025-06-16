#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class DefaultProcess : public Process {
		public:
			DefaultProcess() {
				std::cout << "Hello" << std::endl;
			}

			~DefaultProcess() {
				std::cout << "Bye" << std::endl;
			}
	};
}