#pragma once

#include "pch.hpp"

#include "Engine/base.hpp"

// Math
#include "Engine/lyah.hpp"

// Application
#include "Engine/Application.hpp"
#include "Engine/Layer/Layer.hpp"

// Event
#include "Engine/API/Event/Mouse/MouseDownEvent.hpp"
#include "Engine/API/Event/Mouse/MouseMoveEvent.hpp"
#include "Engine/API/Event/Mouse/MouseUpEvent.hpp"
#include "Engine/API/Event/Mouse/MouseWheelEvent.hpp"
#include "Engine/API/Event/Window/WindowCloseEvent.hpp"
#include "Engine/API/Event/Window/WindowResizeEvent.hpp"

// Camera controller
#include "Engine/Camera/Controller/OrthographicCamera3DController.hpp"

// ECS
#include "Engine/ECS/Component/GUIComponent.hpp"
#include "Engine/ECS/Component/ScriptComponent.hpp"
#include "Engine/ECS/Component/SpriteComponent.hpp"
#include "Engine/ECS/Component/Transform2DComponent.hpp"
#include "Engine/ECS/Component/Transform3DComponent.hpp"
#include "Engine/ECS/ECS.hpp"

// Filesystem
#include "Engine/Filesystem/Decoder/Decoder.hpp"
#include "Engine/Filesystem/File/PNGFile.hpp"
#include "Engine/Filesystem/File/PropertyFile.hpp"
#include "Engine/Filesystem/File/TTFFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/BitStream.hpp"

// GUI
#include "Engine/GUI/GUILayer.hpp"
#include "Engine/GUI/GUIRenderer.hpp"

// Renderer
#include "Engine/API/Rendering/Buffer.hpp"
#include "Engine/API/Rendering/ComputePipeline.hpp"
#include "Engine/API/Rendering/Framebuffer.hpp"
#include "Engine/API/Rendering/RenderPipeline.hpp"
#include "Engine/API/Rendering/Swapchain.hpp"
#include "Engine/API/Rendering/ShaderModule.hpp"
#include "Engine/Renderer/Renderer.hpp"

// Font
#include "Engine/GUI/Font/TrueType/TrueTypeFont.hpp"

// Physics
#include "Engine/API/Physics/GJK2D.hpp"
#include "Engine/API/Physics/GJK2DConditional.hpp"
#include "Engine/API/Physics/GJK2DPoint.hpp"