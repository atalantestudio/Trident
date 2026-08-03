// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/AssetBuilderLibrary.hpp"
#include "AssetBuilder/Process/Material/Lightmapped/PlasterCeiling.hpp"
#include "AssetBuilder/Process/Material/Lightmapped/PlasterWall.hpp"
#include "AssetBuilder/Process/Material/Lightmapped/TileFloor.hpp"
#include "AssetBuilder/Process/Material/Lightmapped/White.hpp"
#include "AssetBuilder/Process/Material/Lightmapped/WoodDoor.hpp"
#include "AssetBuilder/Process/Material/LightmappedPBR/GallineraTable.hpp"
#include "AssetBuilder/Process/Material/PBR/Debug.hpp"
#include "AssetBuilder/Process/Material/PBR/FoldingWoodenStool.hpp"
#include "AssetBuilder/Process/Material/PBR/GallineraTable.hpp"
#include "AssetBuilder/Process/Material/PBR/Lemon.hpp"
#include "AssetBuilder/Process/Material/PBR/RubberDuckToy.hpp"
#include "AssetBuilder/Process/Scene/2D/Boxes.hpp"
#include "AssetBuilder/Process/Scene/2D/Circles.hpp"
#include "AssetBuilder/Process/Scene/2D/Dominos.hpp"
#include "AssetBuilder/Process/Scene/2D/Polygons.hpp"
#include "AssetBuilder/Process/Scene/2D/Pyramid.hpp"
#include "AssetBuilder/Process/Scene/2D/RollingBall.hpp"
#include "AssetBuilder/Process/Scene/2D/Sliding.hpp"
#include "AssetBuilder/Process/Scene/2D/Stacking.hpp"
#include "AssetBuilder/Process/Scene/2D/Swing.hpp"
#include "AssetBuilder/Process/Scene/3D/EdgeContact.hpp"
#include "AssetBuilder/Process/Scene/3D/GallineraTable.hpp"
#include "AssetBuilder/Process/Scene/3D/Hall.hpp"
#include "AssetBuilder/Process/Scene/3D/Lemons.hpp"
#include "AssetBuilder/Process/Scene/3D/PlayerMovement.hpp"
#include "AssetBuilder/Process/Scene/3D/Sliding.hpp"
#include "AssetBuilder/Process/Scene/3D/Stacking.hpp"
#include "AssetBuilder/Process/Scene/3D/Suzanne.hpp"
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

				registerProcess<DebugMaterial>(&assetLibrary);
				registerProcess<FoldingWoodenStoolMaterial>(&assetLibrary);
				registerProcess<GallineraTableMaterial>(&assetLibrary);
				registerProcess<LemonMaterial>(&assetLibrary);
				registerProcess<LPBRGallineraTableMaterial>(&assetLibrary);
				registerProcess<PlasterCeilingMaterial>(&assetLibrary);
				registerProcess<PlasterWallMaterial>(&assetLibrary);
				registerProcess<RubberDuckToyMaterial>(&assetLibrary);
				registerProcess<TileFloorMaterial>(&assetLibrary);
				registerProcess<WhiteMaterial>(&assetLibrary);
				registerProcess<WoodDoorMaterial>(&assetLibrary);

				registerProcess<BoxesScene2D>(&assetLibrary);
				registerProcess<CirclesScene2D>(&assetLibrary);
				registerProcess<DominosScene2D>(&assetLibrary);
				registerProcess<PolygonsScene2D>(&assetLibrary);
				registerProcess<PyramidScene2D>(&assetLibrary);
				registerProcess<RollingBallScene2D>(&assetLibrary);
				registerProcess<SlidingScene2D>(&assetLibrary);
				registerProcess<StackingScene2D>(&assetLibrary);
				registerProcess<SwingScene2D>(&assetLibrary);

				registerProcess<EdgeContactScene3D>(&assetLibrary);
				registerProcess<GallineraTableScene3D>(&assetLibrary);
				registerProcess<HallScene3D>(&assetLibrary);
				registerProcess<LemonsScene3D>(&assetLibrary);
				registerProcess<PlayerMovementScene3D>(&assetLibrary);
				registerProcess<SlidingScene3D>(&assetLibrary);
				registerProcess<StackingScene3D>(&assetLibrary);
				registerProcess<SuzanneScene3D>(&assetLibrary);

				registerProcess<BuildShaderModuleAssetProcess>(&assetLibrary);

				registerProcess<BuildFontAssetProcess>(&assetLibrary);

				registerProcess<BuildAssetMapProcess>(&assetLibrary);

				ENGINE_INFO("Done building assets.");
			}

			void release() final override {
				SceneBuilder::release();
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