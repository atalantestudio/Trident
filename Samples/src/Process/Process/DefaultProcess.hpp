#pragma once

#include "ProjectA.hpp"

#include "Engine/Build/Clusterizer.hpp"
#include "Engine/Filesystem/Asset/FBXToGeometry.hpp"

namespace ProjectA {
	class DefaultProcess : public Process {
		public:
			explicit DefaultProcess() {
				static constexpr char8* fbxPath = "Asset/Model/folding_wooden_stool/folding_wooden_stool.fbx";

				const sequence<uint8> fbxData = Filesystem::openBuffer(fbxPath);
				const File<FileFormat::FBX> fbxFile = Filesystem::decode<FileFormat::FBX>(fbxData);

				Entity3DDescriptor entityDescriptor{};
				entityDescriptor.geometry = FBXToGeometry3D(fbxFile);

				Scene3D scene;

				scene.createEntity(entityDescriptor);

				const ClusteredScene clusteredScene = clusterizeScene(scene);

				BREAKPOINT();
			}
	};
}