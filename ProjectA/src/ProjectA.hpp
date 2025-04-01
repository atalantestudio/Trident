#pragma once

#include "Engine/base.hpp"

// Math
#include "lyah/lyah.hpp"

// Application
#include "Engine/Application.hpp"
#include "Engine/Input.hpp"
#include "Engine/Layer/Layer.hpp"

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
#include "Engine/Filesystem/File/ConfigurationFile.hpp"
#include "Engine/Filesystem/File/PNGFile.hpp"
#include "Engine/Filesystem/File/TTFFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/BitStream.hpp"

// GUI
#include "Engine/GUI/GUILayer.hpp"
#include "Engine/GUI/GUIRenderer.hpp"
#include "Engine/GUI/Object/GUITextObject.hpp"

// ImGui
#include "Engine/ImGui/ImGuiLayer.hpp"

// Renderer
#include "Engine/Renderer/Buffer/Buffer.hpp"
#include "Engine/Renderer/Buffer/BufferUsage.hpp"
#include "Engine/Renderer/Framebuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/RenderPipeline.hpp"
#include "Engine/Renderer/ShaderModule.hpp"
#include "Engine/Renderer/Swapchain.hpp"

// TODO: Include debug demos in a custom engine-side file, in debug mode only.
#include "Engine/Debug/ComputeShaderDemo/ComputeShaderDemoRenderer.hpp"
#include "Engine/Debug/TextureDemo/TextureDemoRenderer.hpp"
#include "Engine/Debug/TriangleDemo/TriangleDemoRenderer.hpp"