// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/entry_point.hpp"

#include "SampleApplication.hpp"

#include "Compute/PrefixSum.hpp"

#include "GUI/Card.hpp"
#include "GUI/Demo1.hpp"
#include "GUI/Demo2.hpp"
#include "GUI/Demo3.hpp"
#include "GUI/DropDownMenu.hpp"
#include "GUI/FrameStatisticsLayer.hpp"
#include "GUI/Launch.hpp"
#include "GUI/OptionMenu.hpp"
#include "GUI/PageMenu.hpp"
#include "GUI/Sample.hpp"
#include "GUI/Test.hpp"
#include "GUI/Text.hpp"
#include "GUI/UTF8.hpp"

#include "Physics2D/Layer/SceneLayer.hpp"

#include "Process/Default.hpp"
#include "Process/ECS.hpp"
#include "Process/SIMD.hpp"

#include "Graphics/Layer/ArrayTexture.hpp"
#include "Graphics/Layer/ClearColor.hpp"
#include "Graphics/Layer/ColorBlend.hpp"
#include "Graphics/Layer/ComputeShader.hpp"
#include "Graphics/Layer/ComputeShaderIndirect.hpp"
#include "Graphics/Layer/DepthTest.hpp"
#include "Graphics/Layer/Empty.hpp"
#include "Graphics/Layer/Gradient.hpp"
#include "Graphics/Layer/ImageAsset.hpp"
#include "Graphics/Layer/InputAttachment.hpp"
#include "Graphics/Layer/MipLevels.hpp"
#include "Graphics/Layer/Multisampling.hpp"
#include "Graphics/Layer/TextureArray.hpp"
#include "Graphics/Layer/Triangle.hpp"
#include "Graphics/Layer/TriangleIndexed.hpp"
#include "Graphics/Layer/TriangleIndexedIndirect.hpp"
#include "Graphics/Layer/TriangleIndirect.hpp"

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
			if (SampleNameProperty == "Card") {
				registerLayer<GUICardLayer>();
			}
			else if (SampleNameProperty == "Demo1") {
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
			else if (SampleNameProperty == "Launch") {
				registerLayer<GUILaunchLayer>();
			}
			else if (SampleNameProperty == "OptionMenu") {
				registerLayer<GUIOptionMenuLayer>();
			}
			else if (SampleNameProperty == "PageMenu") {
				registerLayer<GUIPageMenuLayer>();
			}
			else if (SampleNameProperty == "Sample") {
				registerLayer<GUISampleLayer>();
			}
			else if (SampleNameProperty == "Test") {
				registerLayer<GUITestLayer>();
			}
			else if (SampleNameProperty == "Text") {
				registerLayer<GUITextLayer>();
			}
			else if (SampleNameProperty == "UTF8") {
				registerLayer<GUIUTF8Layer>();
			}

			registerLayer<FrameStatisticsLayer>();
		}
		else if (SampleGroupNameProperty == "Physics2D") {
			registerLayer<Physics2DSceneLayer>();
			registerLayer<FrameStatisticsLayer>();
		}
		else if (SampleGroupNameProperty == "Process") {
			if (SampleNameProperty == "Default") {
				registerProcess<ProcessDefaultProcess>();
			}
			else if (SampleNameProperty == "ECS") {
				registerProcess<ProcessECSProcess>();
			}
			else if (SampleNameProperty == "SIMD") {
				registerProcess<ProcessSIMDProcess>();
			}
		}
		else if (SampleGroupNameProperty == "Graphics") {
			if (SampleNameProperty == "ArrayTexture") {
				registerLayer<SampleArrayTextureLayer>();
			}
			else if (SampleNameProperty == "ClearColor") {
				registerLayer<SampleClearColorLayer>();
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

	Application* createApplication(uint32 argumentCount, char8** argumentValues) {
		ProjectDescriptor projectDescriptor{};
		projectDescriptor.type = ProjectType::MAIN;
		projectDescriptor.name = "Trident Sample";
		projectDescriptor.version = {1, 0, 0};

		return new SampleApplication(projectDescriptor);
	}
}