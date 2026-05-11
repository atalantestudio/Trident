// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "Engine/Application/Application.hpp"

#include "GUI/ECSLayer.hpp"

namespace Trident {
	class EditorApplication : public Application {
		public:
			explicit EditorApplication(const ProjectDescriptor& projectDescriptor, uint32 argumentCount, char8** argumentValues) :
				Application(projectDescriptor)
			{
				setConsoleArguments(argumentCount, argumentValues);

				registerLayer<ECSLayer>();
			}
	};
}