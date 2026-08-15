// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/entry_point.hpp"

#include "SampleApplication.hpp"

#include "Compute/PrefixSum.hpp"

#include "GUI/Article.hpp"
#include "GUI/Card.hpp"
#include "GUI/Demo1.hpp"
#include "GUI/Demo2.hpp"
#include "GUI/Demo3.hpp"
#include "GUI/DropDownMenu.hpp"
#include "GUI/FrameStatisticsLayer.hpp"
#include "GUI/Launch.hpp"
#include "GUI/MenuBar.hpp"
#include "GUI/OptionMenu.hpp"
#include "GUI/PageMenu.hpp"
#include "GUI/PropertyPanel.hpp"
#include "GUI/SliderWidget.hpp"
#include "GUI/Test.hpp"
#include "GUI/Text.hpp"
#include "GUI/TextDebug.hpp"
#include "GUI/TextInput.hpp"
#include "GUI/TextSmoothness.hpp"
#include "GUI/UILayerGroup.hpp"
#include "GUI/UTF8.hpp"

#include "Physics2D/Layer/SceneLayer.hpp"

#include "Process/Default.hpp"
#include "Process/ECS.hpp"
#include "Process/JSON.hpp"
#include "Process/MemoryAllocation.hpp"
#include "Process/SIMD.hpp"

#include "Graphics/Layer/ArrayTexture.hpp"
#include "Graphics/Layer/ClearColor.hpp"
#include "Graphics/Layer/ColorBlend.hpp"
#include "Graphics/Layer/Dispatch.hpp"
#include "Graphics/Layer/DispatchIndirect.hpp"
#include "Graphics/Layer/Empty.hpp"
#include "Graphics/Layer/Gradient.hpp"
#include "Graphics/Layer/ImageAsset.hpp"
#include "Graphics/Layer/InputAttachment.hpp"
#include "Graphics/Layer/MipLevels.hpp"
#include "Graphics/Layer/Multisampling.hpp"
#include "Graphics/Layer/ReversedZ.hpp"
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
			else {
				ENGINE_ASSERT(false, "Unknown sample \"[]\".", SampleNameProperty);
			}
		}
		else if (SampleGroupNameProperty == "GUI") {
			if (SampleNameProperty == "Article") {
				registerLayer<GUIArticleLayer>();
			}
			else if (SampleNameProperty == "Card") {
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
			else if (SampleNameProperty == "PropertyPanel") {
				registerLayer<GUIPropertyPanelLayer>();
			}
			else if (SampleNameProperty == "MenuBar") {
				registerLayer<GUIMenuBarLayer>();
			}
			else if (SampleNameProperty == "OptionMenu") {
				registerLayer<GUIOptionMenuLayer>();
			}
			else if (SampleNameProperty == "PageMenu") {
				registerLayer<GUIPageMenuLayer>();
			}
			else if (SampleNameProperty == "SliderWidget") {
				registerLayer<GUISliderWidgetLayer>();
			}
			else if (SampleNameProperty == "Test") {
				registerLayer<GUITestLayer>();
			}
			else if (SampleNameProperty == "Text") {
				registerLayer<GUITextLayer>();
			}
			else if (SampleNameProperty == "TextDebug") {
				registerLayer<GUITextDebugLayer>();
			}
			else if (SampleNameProperty == "TextInput") {
				registerLayer<GUITextInputLayer>();
			}
			else if (SampleNameProperty == "TextSmoothness") {
				registerLayer<GUITextSmoothnessLayer>();
			}
			else if (SampleNameProperty == "UTF8") {
				registerLayer<GUIUTF8Layer>();
			}
			else {
				ENGINE_ASSERT(false, "Unknown sample \"[]\".", SampleNameProperty);
			}

			registerLayer<FrameStatisticsLayer>();

			registerLayerGroup<UILayerGroup>(range<uint32, uint32>(0, 2));
		}
		else if (SampleGroupNameProperty == "Physics2D") {
			registerLayer<Physics2DSceneLayer>();
			// registerLayer<FrameStatisticsLayer>();

			// TODO(window/layer-group): Multisampled UI layer group.
			registerLayerGroup<LayerGroup>(range<uint32, uint32>(0, 1));
		}
		else if (SampleGroupNameProperty == "Process") {
			if (SampleNameProperty == "Default") {
				registerProcess<ProcessDefaultProcess>();
			}
			else if (SampleNameProperty == "ECS") {
				registerProcess<ProcessECSProcess>();
			}
			else if (SampleNameProperty == "JSON") {
				registerProcess<ProcessJSONProcess>();
			}
			else if (SampleNameProperty == "MemoryAllocation") {
				registerProcess<MemoryAllocationProcess>();
			}
			else if (SampleNameProperty == "SIMD") {
				registerProcess<ProcessSIMDProcess>();
			}
			else {
				ENGINE_ASSERT(false, "Unknown sample \"[]\".", SampleNameProperty);
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
				registerLayer<ColorBlendSampleLayer>();
			}
			else if (SampleNameProperty == "Dispatch") {
				registerLayer<DispatchSampleLayer>();
			}
			else if (SampleNameProperty == "DispatchIndirect") {
				registerLayer<DispatchIndirectSampleLayer>();
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
			else if (SampleNameProperty == "ReversedZ") {
				registerLayer<ReversedZSampleLayer>();
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
			else {
				ENGINE_ASSERT(false, "Unknown sample \"[]\".", SampleNameProperty);
			}

			registerLayerGroup<LayerGroup>(range<uint32, uint32>(0, 1));
		}
		else if (SampleGroupNameProperty == "Sample3D") {
			if (SampleNameProperty == "Forward") {
				registerLayer<Sample3DForwardLayer>();
			}
			else if (SampleNameProperty == "Visibility") {
				registerLayer<Sample3DVisibilityLayer>();
			}
			else {
				ENGINE_ASSERT(false, "Unknown sample \"[]\".", SampleNameProperty);
			}

			registerLayerGroup<LayerGroup>(range<uint32, uint32>(0, 1));
		}
		else {
			ENGINE_ASSERT(false, "Unknown sample group \"[]\".", SampleGroupNameProperty);
		}
	}

	Application* createApplication(uint32 argumentCount, char8** argumentValues) {
		ProjectDescriptor projectDescriptor{};
		projectDescriptor.type = ProjectType::MAIN;
		projectDescriptor.name = "Trident";
		projectDescriptor.version = {1, 0, 0};

		return new SampleApplication(projectDescriptor);
	}
}