#pragma once

#include "ProjectA/base.hpp"

// Math
#include "lyah/lyah.hpp"

// Application
#include "ProjectA/Application.hpp"
#include "ProjectA/Input.hpp"
#include "ProjectA/Layer.hpp"

#ifdef BUILD_MODE_DEBUG
	#include "ProjectA/Logger/Logger.hpp"
#endif

// Camera controller
#include "ProjectA/Camera/Controller/OrthographicCameraController.hpp"

// ImGui
#include "ProjectA/ImGui/ImGuiLayer.hpp"

// Loader
#include "ProjectA/Loader/FileLoader.hpp"

// Renderer
#include "ProjectA/Renderer/2D/Renderer2D.hpp"
#include "ProjectA/Renderer/Framebuffer.hpp"
#include "ProjectA/Renderer/IndexBuffer.hpp"
#include "ProjectA/Renderer/RenderCommand.hpp"
#include "ProjectA/Renderer/RenderPipeline.hpp"
#include "ProjectA/Renderer/Renderer.hpp"
#include "ProjectA/Renderer/ShaderModule.hpp"
// #include "ProjectA/Renderer/ShaderLibrary.hpp"
#include "ProjectA/Renderer/VertexBuffer.hpp"