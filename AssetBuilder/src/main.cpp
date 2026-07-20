// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/BuildAssetMap.hpp"
#include "AssetBuilder/Process/Scene/CabinSceneProcess.hpp"
// #include "AssetBuilder/Process/Scene/HallSceneProcess.hpp"
#include "AssetBuilder/Process/Scene/SnapTestSceneProcess.hpp"
#include "Process/BuildAssetMapProcess.hpp"
#include "Process/BuildFontAssetProcess.hpp"
#include "Process/BuildGeometryAssetProcess.hpp"
#include "Process/BuildImageAssetProcess.hpp"
#include "Process/BuildMaterialAssetProcess.hpp"
#include "Process/BuildSceneAssetProcess.hpp"
#include "Process/BuildShaderModuleAssetProcess.hpp"

#include "Engine/entry_point.hpp"

namespace Trident {
	class AssetBuilderApplication : public Application {
		public:
			explicit AssetBuilderApplication(const ProjectDescriptor& projectDescriptor, uint32 argumentCount, char8** argumentValues) :
				Application(projectDescriptor)
			{
				setConsoleArguments(argumentCount, argumentValues);

				const std::filesystem::path rootOutputPath = static_cast<const char8*>(argumentValues[2]);
				const std::filesystem::path assetMapOutputPath = (rootOutputPath / "Asset" / rootOutputPath.filename()).replace_extension("tam");

				if (std::filesystem::exists(assetMapOutputPath)) {
					AssetLibrary assetLibrary(rootOutputPath);
					assetLibrary.loadAssetMap();

					const std::unordered_map<uint32, view<char8>>& loadedAssetMap = assetLibrary.getAssetMap();

					for (const auto& loadedAsset : loadedAssetMap) {
						assetMap.insert(loadedAsset.first, loadedAsset.second);

						ENGINE_DEBUG("Asset [], path: []", loadedAsset.first, loadedAsset.second);
					}
				}

				registerProcess<BuildGeometryAssetProcess>(&assetMap);
				registerProcess<BuildImageAssetProcess>(&assetMap);

				registerProcess<BuildSceneAssetProcess>(&assetMap);
				registerProcess<CabinSceneProcess>(&assetMap);
				// registerProcess<HallSceneProcess>(&assetMap);
				registerProcess<SnapTestSceneProcess>(&assetMap);

				registerProcess<BuildMaterialAssetProcess>(&assetMap);
				registerProcess<BuildShaderModuleAssetProcess>(&assetMap);
				registerProcess<BuildFontAssetProcess>(&assetMap);
				registerProcess<BuildAssetMapProcess>(&assetMap);
			}

			~AssetBuilderApplication() override {
				ENGINE_INFO("Done building assets.");
			}

		private:
			BuildAssetMap assetMap;
	};

	Application* createApplication(uint32 argumentCount, char8** argumentValues) {
		ProjectDescriptor projectDescriptor{};
		projectDescriptor.type = ProjectType::BUILD;
		projectDescriptor.name = "Trident Asset Builder";

		return new AssetBuilderApplication(projectDescriptor, argumentCount, argumentValues);
	}
}