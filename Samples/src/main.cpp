// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Trident.hpp"

#include "Engine/entry_point.hpp"

// #include "GUI/DemoLayer1.hpp"
// #include "GUI/DemoLayer2.hpp"
// #include "GUI/DemoLayer3.hpp"
// #include "GUI/DropDownMenuLayer.hpp"
// #include "GUI/EventTestLayer.hpp"
// #include "GUI/OptionMenu.hpp"
// #include "GUI/SampleLayer.hpp"
#include "GUI/Text.hpp"
// #include "GUI/UTF8.hpp"

#include "Physics2D/Layer/Sample/Boxes.hpp"
// #include "Physics2D/Layer/Sample/Circles.hpp"
// #include "Physics2D/Layer/Sample/Dominos.hpp"
// #include "Physics2D/Layer/Sample/Empty.hpp"
// #include "Physics2D/Layer/Sample/Polygons.hpp"
// #include "Physics2D/Layer/Sample/Pyramid.hpp"
// #include "Physics2D/Layer/Sample/RollingBall.hpp"
// #include "Physics2D/Layer/Sample/SlidingBoxes.hpp"
// #include "Physics2D/Layer/Sample/Stacking.hpp"
// #include "Physics2D/Layer/Sample/Swing.hpp"
// #include "Physics2D/Layer/GUI/StatisticsLayer.hpp"

#include "Process/DefaultProcess.hpp"

#include "Sample/SampleLayer.hpp"

// #include "Sample3D/Layer/ForwardLayer.hpp"
#include "Sample3D/Layer/VisibilityLayer.hpp"

namespace Trident {
	class SampleApplication : public Application {
		public:
			explicit SampleApplication(const ProjectDescriptor& projectDescriptor) :
				Application(projectDescriptor)
			{
				const SampleGroup SampleGroupProperty = propertyFile->getProperty<SampleGroup>("SampleGroup");

				switch (SampleGroupProperty) {
					case SampleGroup::GUI: registerLayer<SampleGUILayer>(); break;
					case SampleGroup::PHYSICS_2D: {
						registerLayer<SamplePhysics2DLayer>();
						// registerLayer<SamplePhysics2DGUILayer>();

						break;
					}
					case SampleGroup::PROCESS: registerProcess<SampleProcess>(); break;
					case SampleGroup::SAMPLE: registerLayer<SampleLayer>(); break;
					case SampleGroup::SAMPLE_3D: registerLayer<Sample3DLayer>(); break;
				}
			}
	};

	Application* Trident::createApplication(uint32 argumentCount, char8** argumentValues) {
		ProjectDescriptor projectDescriptor{};
		projectDescriptor.type = ProjectType::MAIN;
		projectDescriptor.name = "Trident Sample";
		projectDescriptor.version = {1, 0, 0};

		return new SampleApplication(projectDescriptor);
	}
}