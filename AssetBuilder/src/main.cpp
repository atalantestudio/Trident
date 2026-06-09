// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/entry_point.hpp"
#include "Engine/Filesystem/Asset/AssetMap.hpp"

#include "Process/BuildAssetMapProcess.hpp"
#include "Process/BuildFontAssetProcess.hpp"
#include "Process/BuildGeometryAssetProcess.hpp"
#include "Process/BuildImageAssetProcess.hpp"
#include "Process/BuildSceneAssetProcess.hpp"
#include "Process/BuildShaderModuleAssetProcess.hpp"

namespace Trident {
	class AssetBuilderApplication : public Application {
		public:
			explicit AssetBuilderApplication(const ProjectDescriptor& projectDescriptor, uint32 argumentCount, char8** argumentValues) :
				Application(projectDescriptor)
			{
				setConsoleArguments(argumentCount, argumentValues);

				registerProcess<BuildGeometryAssetProcess>(&assetMap);
				registerProcess<BuildImageAssetProcess>(&assetMap);
				registerProcess<BuildSceneAssetProcess>(&assetMap);
				registerProcess<BuildShaderModuleAssetProcess>(&assetMap);

				// NOTE: Font assets depend on image assets.
				registerProcess<BuildFontAssetProcess>(&assetMap);

				registerProcess<BuildAssetMapProcess>(&assetMap);
			}

			~AssetBuilderApplication() override {
				ENGINE_INFO("Done building assets.");
			}

		private:
			AssetMap assetMap;
	};

	Application* createApplication(uint32 argumentCount, char8** argumentValues) {
		ProjectDescriptor projectDescriptor{};
		projectDescriptor.type = ProjectType::BUILD;
		projectDescriptor.name = "Trident Asset Builder";

		return new AssetBuilderApplication(projectDescriptor, argumentCount, argumentValues);
	}
}