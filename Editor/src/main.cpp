// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "Trident.hpp"

#include "Engine/entry_point.hpp"

namespace Trident {
	class EditorApplication : public Application {
		public:
			explicit EditorApplication(const ProjectDescriptor& projectDescriptor, uint32 argumentCount, char8** argumentValues) :
				Application(projectDescriptor)
			{
				setConsoleArguments(argumentCount, argumentValues);

				// TODO
				ENGINE_TRACE("EditorApplication()");
			}
	};

	Application* createApplication(uint32 argumentCount, char8** argumentValues) {
		ProjectDescriptor projectDescriptor{};
		projectDescriptor.type = ProjectType::MAIN;
		projectDescriptor.name = "Editor";

		return new EditorApplication(projectDescriptor, argumentCount, argumentValues);
	}
}