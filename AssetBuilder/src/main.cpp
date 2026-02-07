// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "Trident.hpp"

#include "Engine/entry_point.hpp"

#include "Process/BuildFontProcess.hpp"
#include "Process/BuildGeometryProcess.hpp"
#include "Process/BuildImageAssetProcess.hpp"
#include "Process/BuildShaderModuleProcess.hpp"

namespace Trident {
	class AssetBuilderApplication : public Application {
		public:
			explicit AssetBuilderApplication(const ProjectDescriptor& projectDescriptor, uint32 argumentCount, char8** argumentValues) :
				Application(projectDescriptor)
			{
				setConsoleArguments(argumentCount, argumentValues);

				registerProcess<BuildGeometryProcess>();
				registerProcess<BuildImageAssetProcess>();
				registerProcess<BuildShaderModuleProcess>();

				// NOTE: Font assets depend on image assets.
				registerProcess<BuildFontProcess>();
			}

			~AssetBuilderApplication() override {
				ENGINE_INFO("Done building assets.");
			}
	};

	Application* createApplication(uint32 argumentCount, char8** argumentValues) {
		ProjectDescriptor projectDescriptor{};
		projectDescriptor.type = ProjectType::BUILD;
		projectDescriptor.name = "Trident Asset Builder";

		return new AssetBuilderApplication(projectDescriptor, argumentCount, argumentValues);
	}
}