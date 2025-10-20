#include "pch.hpp"

#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Process/TSCProcess.hpp"

namespace ProjectA {
	class TSCApplication : public Application {
		public:
			explicit TSCApplication(const ProjectDescriptor& projectDescriptor, uint32 argumentCount, char8** argumentValues) :
				Application(projectDescriptor)
			{
				setConsoleArguments(argumentCount, argumentValues);

				registerProcess<TSCProcess>(getConsoleArguments());
			}
	};

	Application* ProjectA::createApplication(uint32 argumentCount, char8** argumentValues) {
		ProjectDescriptor projectDescriptor{};
		projectDescriptor.type = ProjectType::BUILD;
		projectDescriptor.name = "TSL Transpiler";
		projectDescriptor.version = {1, 0, 0};

		return new TSCApplication(projectDescriptor, argumentCount, argumentValues);
	}
}