// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/entry_point.hpp"

#include "SampleApplication.hpp"

#include "Compute/PrefixSum.hpp"

#include "GUI/Demo1.hpp"
#include "GUI/Demo2.hpp"
#include "GUI/Demo3.hpp"
#include "GUI/DropDownMenu.hpp"
#include "GUI/EventTest.hpp"
#include "GUI/OptionMenu.hpp"
#include "GUI/Sample.hpp"
#include "GUI/Text.hpp"
#include "GUI/UTF8.hpp"

#include "Physics2D/Layer/Sample/Boxes.hpp"
#include "Physics2D/Layer/Sample/Circles.hpp"
#include "Physics2D/Layer/Sample/ContactTest.hpp"
#include "Physics2D/Layer/Sample/Dominos.hpp"
#include "Physics2D/Layer/Sample/Empty.hpp"
#include "Physics2D/Layer/Sample/Polygons.hpp"
#include "Physics2D/Layer/Sample/Pyramid.hpp"
#include "Physics2D/Layer/Sample/RollingBall.hpp"
#include "Physics2D/Layer/Sample/SlidingBoxes.hpp"
#include "Physics2D/Layer/Sample/Stacking.hpp"
#include "Physics2D/Layer/Sample/Swing.hpp"
#include "Physics2D/Layer/GUI/StatisticsLayer.hpp"

#include "Process/Default.hpp"

#include "Sample/Layer/ArrayTexture.hpp"
#include "Sample/Layer/ColorBlend.hpp"
#include "Sample/Layer/ComputeShader.hpp"
#include "Sample/Layer/ComputeShaderIndirect.hpp"
#include "Sample/Layer/DepthTest.hpp"
#include "Sample/Layer/Empty.hpp"
#include "Sample/Layer/Gradient.hpp"
#include "Sample/Layer/Image.hpp"
#include "Sample/Layer/ImageAsset.hpp"
#include "Sample/Layer/InputAttachment.hpp"
#include "Sample/Layer/MipLevels.hpp"
#include "Sample/Layer/Multisampling.hpp"
#include "Sample/Layer/TextureArray.hpp"
#include "Sample/Layer/Triangle.hpp"
#include "Sample/Layer/TriangleIndexed.hpp"
#include "Sample/Layer/TriangleIndexedIndirect.hpp"
#include "Sample/Layer/TriangleIndirect.hpp"

#include "Sample3D/Layer/ForwardLayer.hpp"
#include "Sample3D/Layer/VisibilityLayer.hpp"

namespace Trident {
	SampleApplication::SampleApplication(const ProjectDescriptor& projectDescriptor) :
		Application(projectDescriptor)
	{
		const view<char8> SampleGroupNameProperty = propertyFile->getProperty<view<char8>>("SampleGroupName");
		const view<char8> SampleNameProperty = propertyFile->getProperty<view<char8>>("SampleName");

		if (SampleGroupNameProperty == "Compute") {
			if (SampleNameProperty == "PrefixSum") {
				registerProcess<ComputePrefixSumProcess>();
			}
		}
		else if (SampleGroupNameProperty == "GUI") {
			if (SampleNameProperty == "Demo1") {
				registerLayer<GUIDemo1Layer>();
			}
			else if (SampleNameProperty == "Demo2") {
				registerLayer<GUIDemo2Layer>();
			}
			else if (SampleNameProperty == "Demo3") {
				registerLayer<GUIDemo3Layer>();
			}
			else if (SampleNameProperty == "DropDownMenu") {
				registerLayer<GUIDropDownMenuLayer>();
			}
			else if (SampleNameProperty == "EventTest") {
				registerLayer<GUIEventTestLayer>();
			}
			else if (SampleNameProperty == "OptionMenu") {
				registerLayer<GUIOptionMenuLayer>();
			}
			else if (SampleNameProperty == "Sample") {
				registerLayer<GUISampleLayer>();
			}
			else if (SampleNameProperty == "Text") {
				registerLayer<GUITextLayer>();
			}
			else if (SampleNameProperty == "UTF8") {
				registerLayer<GUIUTF8Layer>();
			}
		}
		else if (SampleGroupNameProperty == "Physics2D") {
			if (SampleNameProperty == "Boxes") {
				registerLayer<Physics2DBoxesLayer>();
			}
			else if (SampleNameProperty == "Circles") {
				registerLayer<Physics2DCirclesLayer>();
			}
			else if (SampleNameProperty == "ContactTest") {
				registerLayer<Physics2DContactTestLayer>();
			}
			else if (SampleNameProperty == "Dominos") {
				registerLayer<Physics2DDominosLayer>();
			}
			else if (SampleNameProperty == "Empty") {
				registerLayer<Physics2DEmptyLayer>();
			}
			else if (SampleNameProperty == "Polygons") {
				registerLayer<Physics2DPolygonsLayer>();
			}
			else if (SampleNameProperty == "Pyramid") {
				registerLayer<Physics2DPyramidLayer>();
			}
			else if (SampleNameProperty == "RollingBall") {
				registerLayer<Physics2DRollingBallLayer>();
			}
			else if (SampleNameProperty == "SlidingBoxes") {
				registerLayer<Physics2DSlidingBoxesLayer>();
			}
			else if (SampleNameProperty == "Stacking") {
				registerLayer<Physics2DStackingLayer>();
			}
			else if (SampleNameProperty == "Swing") {
				registerLayer<Physics2DSwingLayer>();
			}

			registerLayer<Physics2DGUIStatisticsLayer>();
		}
		else if (SampleGroupNameProperty == "Process") {
			if (SampleNameProperty == "Default") {
				registerProcess<ProcessDefaultProcess>();
			}
		}
		else if (SampleGroupNameProperty == "Sample") {
			if (SampleNameProperty == "ArrayTexture") {
				registerLayer<SampleArrayTextureLayer>();
			}
			else if (SampleNameProperty == "ColorBlend") {
				registerLayer<SampleColorBlendLayer>();
			}
			else if (SampleNameProperty == "ComputeShader") {
				registerLayer<SampleComputeShaderLayer>();
			}
			else if (SampleNameProperty == "ComputeShaderIndirect") {
				registerLayer<SampleComputeShaderIndirectLayer>();
			}
			else if (SampleNameProperty == "DepthTest") {
				registerLayer<SampleDepthTestLayer>();
			}
			else if (SampleNameProperty == "Empty") {
				registerLayer<SampleEmptyLayer>();
			}
			else if (SampleNameProperty == "Gradient") {
				registerLayer<SampleGradientLayer>();
			}
			else if (SampleNameProperty == "Image") {
				registerLayer<SampleImageLayer>();
			}
			else if (SampleNameProperty == "ImageAsset") {
				registerLayer<SampleImageAssetLayer>();
			}
			else if (SampleNameProperty == "InputAttachment") {
				registerLayer<SampleInputAttachmentLayer>();
			}
			else if (SampleNameProperty == "MipLevels") {
				registerLayer<SampleMipLevelsLayer>();
			}
			else if (SampleNameProperty == "Multisampling") {
				registerLayer<SampleMultisamplingLayer>();
			}
			else if (SampleNameProperty == "TextureArray") {
				registerLayer<SampleTextureArrayLayer>();
			}
			else if (SampleNameProperty == "Triangle") {
				registerLayer<SampleTriangleLayer>();
			}
			else if (SampleNameProperty == "TriangleIndexed") {
				registerLayer<SampleTriangleIndexedLayer>();
			}
			else if (SampleNameProperty == "TriangleIndexedIndirect") {
				registerLayer<SampleTriangleIndexedIndirectLayer>();
			}
			else if (SampleNameProperty == "TriangleIndirect") {
				registerLayer<SampleTriangleIndirectLayer>();
			}
		}
		else if (SampleGroupNameProperty == "Sample3D") {
			if (SampleNameProperty == "Forward") {
				registerLayer<Sample3DForwardLayer>();
			}
			else if (SampleNameProperty == "Visibility") {
				registerLayer<Sample3DVisibilityLayer>();
			}
		}
	}

	Application* Trident::createApplication(uint32 argumentCount, char8** argumentValues) {
		ProjectDescriptor projectDescriptor{};
		projectDescriptor.type = ProjectType::MAIN;
		projectDescriptor.name = "Trident Sample";
		projectDescriptor.version = {1, 0, 0};

		return new SampleApplication(projectDescriptor);
	}
}