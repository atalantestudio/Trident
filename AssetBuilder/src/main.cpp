// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/AssetBuilderLibrary.hpp"
#include "AssetBuilder/Process/Geometry/2D/Polygon.hpp"
#include "AssetBuilder/Process/Geometry/2D/Triangle.hpp"
#include "AssetBuilder/Process/Geometry/2D/UnitCircle.hpp"
#include "AssetBuilder/Process/Geometry/2D/UnitSquare.hpp"
#include "AssetBuilder/Process/Geometry/3D/RectangularCuboid.hpp"
#include "AssetBuilder/Process/Geometry/3D/SingleTextureUnitBox.hpp"
#include "AssetBuilder/Process/Geometry/3D/UnitBox.hpp"
#include "AssetBuilder/Process/Geometry/3D/UnitPlane.hpp"
#include "AssetBuilder/Process/Material/Lightmapped/Default.hpp"
#include "AssetBuilder/Process/Material/Lightmapped/PlasterCeiling.hpp"
#include "AssetBuilder/Process/Material/Lightmapped/PlasterWall.hpp"
#include "AssetBuilder/Process/Material/Lightmapped/TileFloor.hpp"
#include "AssetBuilder/Process/Material/Lightmapped/WoodDoor.hpp"
#include "AssetBuilder/Process/Material/LightmappedPBR/GallineraTable.hpp"
#include "AssetBuilder/Process/Material/PBR/Debug.hpp"
#include "AssetBuilder/Process/Material/PBR/Default.hpp"
#include "AssetBuilder/Process/Material/PBR/FoldingWoodenStool.hpp"
#include "AssetBuilder/Process/Material/PBR/GallineraTable.hpp"
#include "AssetBuilder/Process/Material/PBR/Lemon.hpp"
#include "AssetBuilder/Process/Material/PBR/RoundWoodenTable01.hpp"
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
#include "AssetBuilder/Process/Scene/3D/Cubes.hpp"
#include "AssetBuilder/Process/Scene/3D/EdgeContact.hpp"
#include "AssetBuilder/Process/Scene/3D/GallineraTable.hpp"
#include "AssetBuilder/Process/Scene/3D/Hall.hpp"
#include "AssetBuilder/Process/Scene/3D/Lemons.hpp"
#include "AssetBuilder/Process/Scene/3D/PBRTest.hpp"
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

				Lightmapper::adapter = &graphicsAdapter;

				registerProcesses();

				ENGINE_INFO("Done building assets.");
			}

			void release() final override {
				SceneBuilder::release();
			}

		private:
			void registerProcesses() {
				registerProcess<BuildGeometryAssetProcess>(&assetLibrary);

				registerProcess<PolygonGeometry2D>(&assetLibrary);
				registerProcess<TriangleGeometry2D>(&assetLibrary);
				registerProcess<UnitCircleGeometry2D>(&assetLibrary);
				registerProcess<UnitSquareGeometry2D>(&assetLibrary);

				registerProcess<RectangularCuboidGeometry3D>(&assetLibrary);
				registerProcess<SingleTextureUnitBoxGeometry3D>(&assetLibrary);
				registerProcess<UnitBoxGeometry3D>(&assetLibrary);
				registerProcess<UnitPlaneGeometry3D>(&assetLibrary);

				registerProcess<BuildImageAssetProcess>(&assetLibrary);

				registerProcess<DebugMaterial>(&assetLibrary);
				registerProcess<FoldingWoodenStoolMaterial>(&assetLibrary);
				registerProcess<GallineraTableMaterial>(&assetLibrary);
				registerProcess<LemonMaterial>(&assetLibrary);
				registerProcess<LMDefaultMaterial>(&assetLibrary);
				registerProcess<LPBRGallineraTableMaterial>(&assetLibrary);
				registerProcess<PBRDefaultMaterial>(&assetLibrary);
				registerProcess<PlasterCeilingMaterial>(&assetLibrary);
				registerProcess<PlasterWallMaterial>(&assetLibrary);
				registerProcess<RoundWoodenTable01Material>(&assetLibrary);
				registerProcess<RubberDuckToyMaterial>(&assetLibrary);
				registerProcess<TileFloorMaterial>(&assetLibrary);
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

				registerProcess<CubesScene3D>(&assetLibrary);
				registerProcess<EdgeContactScene3D>(&assetLibrary);
				registerProcess<GallineraTableScene3D>(&assetLibrary);
				registerProcess<HallScene3D>(&assetLibrary);
				registerProcess<LemonsScene3D>(&assetLibrary);
				registerProcess<PBRTestScene3D>(&assetLibrary);
				registerProcess<PlayerMovementScene3D>(&assetLibrary);
				registerProcess<SlidingScene3D>(&assetLibrary);
				registerProcess<StackingScene3D>(&assetLibrary);
				registerProcess<SuzanneScene3D>(&assetLibrary);

				registerProcess<BuildShaderModuleAssetProcess>(&assetLibrary);

				registerProcess<BuildFontAssetProcess>(&assetLibrary);

				registerProcess<BuildAssetMapProcess>(&assetLibrary);
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