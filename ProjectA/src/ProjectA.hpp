#pragma once

#include "Engine/base.hpp"

// Math
#include "lyah/lyah.hpp"

// Application
#include "Engine/Application.hpp"
#include "Engine/Input.hpp"
#include "Engine/Layer.hpp"

#ifdef BUILD_MODE_DEBUG
	// Logger
	#include "Engine/Logger/Logger.hpp"
#endif

// Camera controller
#include "Engine/Camera/Controller/OrthographicCameraController.hpp"

// ImGui
#include "Engine/ImGui/ImGuiLayer.hpp"

// Loader
#include "Engine/Loader/FileLoader.hpp"

// Renderer
#include "Engine/Renderer/2D/Renderer2D.hpp"
#include "Engine/Renderer/Framebuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/RenderCommand.hpp"
#include "Engine/Renderer/RenderPipeline.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/ShaderModule.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"