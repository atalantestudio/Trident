#pragma once

#include "Engine/base.hpp"

// Math
#include "lyah/lyah.hpp"

// Application
#include "Engine/Application.hpp"
#include "Engine/Input.hpp"
#include "Engine/Layer.hpp"

// 3D camera controller
#include "Engine/Camera/Controller/OrthographicCamera3DController.hpp"

// ECS
#include "Engine/ECS/Component/GUIComponent.hpp"
#include "Engine/ECS/Component/SpriteComponent.hpp"
#include "Engine/ECS/Component/Transform2DComponent.hpp"
#include "Engine/ECS/Component/Transform3DComponent.hpp"
#include "Engine/ECS/ECS.hpp"

// Filesystem
#include "Engine/Filesystem/PNGFileLoader.hpp"

// GUI
#include "Engine/Renderer/GUI/GUIRenderer.hpp"

// ImGui
#include "Engine/ImGui/ImGuiLayer.hpp"

// Renderer
#include "Engine/Renderer/2D/Renderer2D.hpp"
#include "Engine/Renderer/Buffer/Buffer.hpp"
#include "Engine/Renderer/Buffer/BufferUsage.hpp"
#include "Engine/Renderer/Framebuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/RenderPipeline.hpp"
#include "Engine/Renderer/ShaderModule.hpp"
#include "Engine/Renderer/Swapchain.hpp"

// Scene
#include "Engine/Scene/Scene.hpp"

#ifdef BUILD_MODE_DEBUG
	#include "Engine/Renderer/TriangleDemo/TriangleDemoRenderer.hpp"
#endif