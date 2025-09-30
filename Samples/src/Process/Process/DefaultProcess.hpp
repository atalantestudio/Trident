#pragma once

#include "ProjectA.hpp"

#include "Engine/Build/Clusterizer.hpp"
#include "Engine/Filesystem/Asset/FBXToGeometry.hpp"

namespace ProjectA {
	class DefaultProcess : public Process {
		public:
			explicit DefaultProcess() {
				BREAKPOINT();
			}
	};
}