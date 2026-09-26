// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/AssetBuilderLibrary.hpp"
#include "AssetBuilder/API/Font/GlyphRasterizer.hpp"
#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"
#include "AssetBuilder/Process/Font/DefaultFont.hpp"
#include "AssetBuilder/Process/Geometry/2D/Polygon.hpp"
#include "AssetBuilder/Process/Geometry/2D/Triangle.hpp"
#include "AssetBuilder/Process/Geometry/2D/UnitCircle.hpp"
#include "AssetBuilder/Process/Geometry/2D/UnitSquare.hpp"
#include "AssetBuilder/Process/Geometry/3D/RectangularCuboid.hpp"
#include "AssetBuilder/Process/Geometry/3D/SingleTextureUnitBox.hpp"
#include "AssetBuilder/Process/Geometry/3D/UnitBox.hpp"
#include "AssetBuilder/Process/Geometry/3D/UnitPlane.hpp"
#include "AssetBuilder/Process/Geometry/3D/UnitSphere.hpp"
#include "AssetBuilder/Process/Image/EveningRoad01.hpp"
#include "AssetBuilder/Process/Image/LemonARMMap.hpp"
#include "AssetBuilder/Process/Image/OvercastSoil.hpp"
#include "AssetBuilder/Process/Image/Quiver.hpp"
#include "AssetBuilder/Process/Material/DDGI/DDGI.hpp"
#include "AssetBuilder/Process/Material/Lightmapped/DebugGray.hpp"
#include "AssetBuilder/Process/Material/Lightmapped/DebugOrange.hpp"
#include "AssetBuilder/Process/Material/Lightmapped/DebugWall.hpp"
#include "AssetBuilder/Process/Material/Lightmapped/Default.hpp"
#include "AssetBuilder/Process/Material/Lightmapped/PlasterCeiling.hpp"
#include "AssetBuilder/Process/Material/Lightmapped/PlasterWall.hpp"
#include "AssetBuilder/Process/Material/Lightmapped/TileFloor.hpp"
#include "AssetBuilder/Process/Material/Lightmapped/WoodDoor.hpp"
#include "AssetBuilder/Process/Material/LightmappedPBR/GallineraTable.hpp"
#include "AssetBuilder/Process/Material/LightmappedPBR/PowerBox01.hpp"
#include "AssetBuilder/Process/Material/LightmappedPBR/UtilityBox01.hpp"
#include "AssetBuilder/Process/Material/LightmappedPBR/UtilityBox02.hpp"
#include "AssetBuilder/Process/Material/LightmappedPBR/WetFloorSign01.hpp"
#include "AssetBuilder/Process/Material/PBR/BlueMetalPlate.hpp"
#include "AssetBuilder/Process/Material/PBR/Debug.hpp"
#include "AssetBuilder/Process/Material/PBR/DebugDefault.hpp"
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
#include "AssetBuilder/Process/Scene/3D/CornellBox.hpp"
#include "AssetBuilder/Process/Scene/3D/Cubes.hpp"
#include "AssetBuilder/Process/Scene/3D/DDGIRef0.hpp"
#include "AssetBuilder/Process/Scene/3D/DDGITest.hpp"
#include "AssetBuilder/Process/Scene/3D/DebugPlayerMovement.hpp"
#include "AssetBuilder/Process/Scene/3D/Dragon.hpp"
#include "AssetBuilder/Process/Scene/3D/EdgeContact.hpp"
#include "AssetBuilder/Process/Scene/3D/FaceTest.hpp"
#include "AssetBuilder/Process/Scene/3D/GallineraTable.hpp"
#include "AssetBuilder/Process/Scene/3D/Hall.hpp"
#include "AssetBuilder/Process/Scene/3D/HallReference.hpp"
#include "AssetBuilder/Process/Scene/3D/Lemons.hpp"
#include "AssetBuilder/Process/Scene/3D/LightmapTest.hpp"
#include "AssetBuilder/Process/Scene/3D/PBRTest.hpp"
#include "AssetBuilder/Process/Scene/3D/PropGallery.hpp"
#include "AssetBuilder/Process/Scene/3D/Sliding.hpp"
#include "AssetBuilder/Process/Scene/3D/Stacking.hpp"
#include "AssetBuilder/Process/Scene/3D/Suzanne.hpp"
#include "AssetBuilder/Process/Scene/3D/WetFloorSign.hpp"
#include "Process/BuildAssetMapProcess.hpp"
#include "Process/BuildFontAssetProcess.hpp"
#include "Process/BuildGeometryAssetProcess.hpp"
#include "Process/BuildShaderModuleAssetProcess.hpp"
#include "Process/CopyImageProcess.hpp"

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

				glyphRasterizer.initialize(graphicsAdapter);
				lightmapper.initialize(graphicsAdapter);

				registerProcesses();
			}

		private:
			void registerProcesses() {
				allocateProcesses(70);

				registerProcess<BuildGeometryAssetProcess>(&assetLibrary);

				registerProcess<PolygonGeometry2D>(&assetLibrary);
				registerProcess<TriangleGeometry2D>(&assetLibrary);
				registerProcess<UnitCircleGeometry2D>(&assetLibrary);
				registerProcess<UnitSquareGeometry2D>(&assetLibrary);

				registerProcess<RectangularCuboidGeometry3D>(&assetLibrary);
				registerProcess<SingleTextureUnitBoxGeometry3D>(&assetLibrary);
				registerProcess<UnitBoxGeometry3D>(&assetLibrary);
				registerProcess<UnitPlaneGeometry3D>(&assetLibrary);
				registerProcess<UnitSphereGeometry3D>(&assetLibrary);

				registerProcess<CopyImageProcess>(&assetLibrary);
				registerProcess<EveningRoad01CubeMap>(&assetLibrary);
				registerProcess<LemonARMMap>(&assetLibrary);
				registerProcess<OvercastSoilCubeMap>(&assetLibrary);
				registerProcess<QuiverImage>(&assetLibrary);

				registerProcess<DefaultFont>(&assetLibrary);
				registerProcess<BuildFontAssetProcess>(&assetLibrary, &glyphRasterizer);

				registerProcess<BuildShaderModuleAssetProcess>(&assetLibrary);

				registerProcess<DDGIMaterial>(&assetLibrary);
				registerProcess<DebugMaterial>(&assetLibrary);
				registerProcess<FoldingWoodenStoolMaterial>(&assetLibrary);
				registerProcess<GallineraTableMaterial>(&assetLibrary);
				registerProcess<LemonMaterial>(&assetLibrary);
				registerProcess<LMDefaultMaterial>(&assetLibrary);
				registerProcess<LMDebugGrayMaterial>(&assetLibrary);
				registerProcess<LMDebugOrangeMaterial>(&assetLibrary);
				registerProcess<LMDebugWallMaterial>(&assetLibrary);
				registerProcess<LMPlasterCeilingMaterial>(&assetLibrary);
				registerProcess<LMPlasterWallMaterial>(&assetLibrary);
				registerProcess<LMTileFloorMaterial>(&assetLibrary);
				registerProcess<LMWoodDoorMaterial>(&assetLibrary);
				registerProcess<LPBRGallineraTableMaterial>(&assetLibrary);
				registerProcess<PBRBlueMetalPlateMaterial>(&assetLibrary);
				registerProcess<PBRDebugDefaultMaterial>(&assetLibrary);
				registerProcess<PBRDefaultMaterial>(&assetLibrary);
				registerProcess<PowerBox01Material>(&assetLibrary);
				registerProcess<RoundWoodenTable01Material>(&assetLibrary);
				registerProcess<RubberDuckToyMaterial>(&assetLibrary);
				registerProcess<UtilityBox01Material>(&assetLibrary);
				registerProcess<UtilityBox02Material>(&assetLibrary);
				registerProcess<WetFloorSign01Material>(&assetLibrary);

				registerProcess<BoxesScene2D>(&assetLibrary);
				registerProcess<CirclesScene2D>(&assetLibrary);
				registerProcess<DominosScene2D>(&assetLibrary);
				registerProcess<PolygonsScene2D>(&assetLibrary);
				registerProcess<PyramidScene2D>(&assetLibrary);
				registerProcess<RollingBallScene2D>(&assetLibrary);
				registerProcess<SlidingScene2D>(&assetLibrary);
				registerProcess<StackingScene2D>(&assetLibrary);
				registerProcess<SwingScene2D>(&assetLibrary);

				registerProcess<CornellBoxScene3D>(&assetLibrary, &lightmapper);
				registerProcess<CubesScene3D>(&assetLibrary, &lightmapper);
				registerProcess<DDGIRef0Scene3D>(&assetLibrary, &lightmapper);
				registerProcess<DDGITestScene3D>(&assetLibrary, &lightmapper);
				registerProcess<DebugPlayerMovementMap>(&assetLibrary, &lightmapper);
				registerProcess<DragonScene3D>(&assetLibrary, &lightmapper);
				registerProcess<EdgeContactScene3D>(&assetLibrary, &lightmapper);
				registerProcess<FaceTestScene3D>(&assetLibrary, &lightmapper);
				registerProcess<GallineraTableScene3D>(&assetLibrary, &lightmapper);
				registerProcess<HallMap>(&assetLibrary, &lightmapper);
				registerProcess<HallReferenceMap>(&assetLibrary, &lightmapper);
				registerProcess<LemonsScene3D>(&assetLibrary, &lightmapper);
				registerProcess<LightmapTestScene3D>(&assetLibrary, &lightmapper);
				registerProcess<PBRTestScene3D>(&assetLibrary, &lightmapper);
				registerProcess<PropGalleryScene3D>(&assetLibrary, &lightmapper);
				registerProcess<SlidingScene3D>(&assetLibrary, &lightmapper);
				registerProcess<StackingScene3D>(&assetLibrary, &lightmapper);
				registerProcess<SuzanneScene3D>(&assetLibrary, &lightmapper);
				registerProcess<WetFloorSignScene3D>(&assetLibrary, &lightmapper);

				registerProcess<BuildAssetMapProcess>(&assetLibrary);
			}

		private:
			AssetBuilderLibrary assetLibrary;
			GlyphRasterizer glyphRasterizer;
			Lightmapper lightmapper;
	};

	Application* createApplication(uint32 argumentCount, char8** argumentValues) {
		ProjectDescriptor projectDescriptor{};
		projectDescriptor.type = ProjectType::BUILD;
		projectDescriptor.name = "Trident Asset Builder";

		return new AssetBuilderApplication(projectDescriptor, argumentCount, argumentValues);
	}
}