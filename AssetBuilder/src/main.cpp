// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/AssetBuilderLibrary.hpp"
#include "AssetBuilder/Process/Material/LightmappedWhiteMaterialProcess.hpp"
#include "AssetBuilder/Process/Material/PlasterCeilingMaterialProcess.hpp"
#include "AssetBuilder/Process/Material/PlasterWallMaterialProcess.hpp"
#include "AssetBuilder/Process/Material/TileFloorMaterialProcess.hpp"
#include "AssetBuilder/Process/Material/WoodDoorMaterialProcess.hpp"
#include "AssetBuilder/Process/Scene/BoxesSceneProcess.hpp"
#include "AssetBuilder/Process/Scene/Boxes3DSceneProcess.hpp"
#include "AssetBuilder/Process/Scene/BoxStackingSceneProcess.hpp"
#include "AssetBuilder/Process/Scene/CabinSceneProcess.hpp"
#include "AssetBuilder/Process/Scene/CirclesSceneProcess.hpp"
#include "AssetBuilder/Process/Scene/DominosSceneProcess.hpp"
#include "AssetBuilder/Process/Scene/EdgeContactSceneProcess.hpp"
#include "AssetBuilder/Process/Scene/HallSceneProcess.hpp"
#include "AssetBuilder/Process/Scene/LemonsSceneProcess.hpp"
#include "AssetBuilder/Process/Scene/PlayerMovementSceneProcess.hpp"
#include "AssetBuilder/Process/Scene/PolygonsSceneProcess.hpp"
#include "AssetBuilder/Process/Scene/PyramidSceneProcess.hpp"
#include "AssetBuilder/Process/Scene/RollingBallSceneProcess.hpp"
#include "AssetBuilder/Process/Scene/SlidingBoxesSceneProcess.hpp"
#include "AssetBuilder/Process/Scene/SnapTestSceneProcess.hpp"
#include "AssetBuilder/Process/Scene/StackingSceneProcess.hpp"
#include "AssetBuilder/Process/Scene/SuzanneSceneProcess.hpp"
#include "AssetBuilder/Process/Scene/SwingSceneProcess.hpp"
#include "Process/BuildAssetMapProcess.hpp"
#include "Process/BuildFontAssetProcess.hpp"
#include "Process/BuildGeometryAssetProcess.hpp"
#include "Process/BuildImageAssetProcess.hpp"
#include "Process/BuildShaderModuleAssetProcess.hpp"

#include "Engine/entry_point.hpp"

namespace Trident {
	class AssetBuilderApplication : public Application {
		public:
			explicit AssetBuilderApplication(const ProjectDescriptor& projectDescriptor, uint32 argumentCount, char8** argumentValues) :
				Application(projectDescriptor),
				assetLibrary(static_cast<const char8*>(argumentValues[2]))
			{
				setConsoleArguments(argumentCount, argumentValues);

				const std::filesystem::path rootOutputPath = static_cast<const char8*>(argumentValues[2]);
				const std::filesystem::path assetMapOutputPath = (rootOutputPath / "Asset" / rootOutputPath.filename()).replace_extension("tam");

				if (std::filesystem::exists(assetMapOutputPath)) {
					assetLibrary.loadAssetMap();

					for (const auto& loadedAsset : assetLibrary.getAssetMap()) {
						assetLibrary.registerAsset(loadedAsset.first, loadedAsset.second);
					}
				}

				registerProcess<BuildGeometryAssetProcess>(&assetLibrary);

				registerProcess<BuildImageAssetProcess>(&assetLibrary);

				registerProcess<LightmappedWhiteMaterialProcess>(&assetLibrary);
				registerProcess<PlasterCeilingMaterialProcess>(&assetLibrary);
				registerProcess<PlasterWallMaterialProcess>(&assetLibrary);
				registerProcess<TileFloorMaterialProcess>(&assetLibrary);
				registerProcess<WoodDoorMaterialProcess>(&assetLibrary);

				registerProcess<BoxesSceneProcess>(&assetLibrary);
				registerProcess<CirclesSceneProcess>(&assetLibrary);
				registerProcess<DominosSceneProcess>(&assetLibrary);
				registerProcess<PolygonsSceneProcess>(&assetLibrary);
				registerProcess<PyramidSceneProcess>(&assetLibrary);
				registerProcess<RollingBallSceneProcess>(&assetLibrary);
				registerProcess<SlidingBoxesSceneProcess>(&assetLibrary);
				registerProcess<StackingSceneProcess>(&assetLibrary);
				registerProcess<SwingSceneProcess>(&assetLibrary);

				registerProcess<Boxes3DSceneProcess>(&assetLibrary);
				registerProcess<BoxStackingSceneProcess>(&assetLibrary);
				registerProcess<CabinSceneProcess>(&assetLibrary);
				registerProcess<EdgeContactSceneProcess>(&assetLibrary);
				registerProcess<HallSceneProcess>(&assetLibrary);
				registerProcess<LemonsSceneProcess>(&assetLibrary);
				registerProcess<PlayerMovementSceneProcess>(&assetLibrary);
				registerProcess<SnapTestSceneProcess>(&assetLibrary);
				registerProcess<SuzanneSceneProcess>(&assetLibrary);

				registerProcess<BuildShaderModuleAssetProcess>(&assetLibrary);

				registerProcess<BuildFontAssetProcess>(&assetLibrary);

				registerProcess<BuildAssetMapProcess>(&assetLibrary);

				ENGINE_INFO("Done building assets.");
			}

			void release() final override {
				StaticSceneBuilder::lightmapper.~Lightmapper();
			}

		private:
			AssetBuilderLibrary assetLibrary;
	};

	Application* createApplication(uint32 argumentCount, char8** argumentValues) {
		ProjectDescriptor projectDescriptor{};
		projectDescriptor.type = ProjectType::BUILD;
		projectDescriptor.name = "Trident Asset Builder";

		return new AssetBuilderApplication(projectDescriptor, argumentCount, argumentValues);
	}
}