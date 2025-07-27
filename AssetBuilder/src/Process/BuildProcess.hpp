#pragma once

namespace ProjectA {
	class BuildProcess : public Process {
		public:
			explicit BuildProcess() {
				BREAKPOINT();
			}
	};
}