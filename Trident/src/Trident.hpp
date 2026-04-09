// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

// TODO: Don't include the precompiled header here.
#include "pch.hpp"

// Application
#include "Engine/Application/Application.hpp"
#include "Engine/Layer/Layer.hpp"

// Event
#include "Engine/API/Event/EventDispatcher.hpp"
#include "Engine/API/Event/Keyboard/KeyDownEvent.hpp"
#include "Engine/API/Event/Keyboard/KeyUpEvent.hpp"
#include "Engine/API/Event/Mouse/MouseDownEvent.hpp"
#include "Engine/API/Event/Mouse/MouseMoveEvent.hpp"
#include "Engine/API/Event/Mouse/MouseUpEvent.hpp"
#include "Engine/API/Event/Mouse/MouseWheelEvent.hpp"
#include "Engine/API/Event/Window/WindowCloseEvent.hpp"
#include "Engine/API/Event/Window/WindowResizeEvent.hpp"

// ECS
#include "Engine/ECS/Component/GeometryComponent.hpp"
#include "Engine/ECS/Component/ScriptComponent.hpp"
#include "Engine/ECS/ECS.hpp"

// Filesystem
#include "Engine/Filesystem/File/PropertyFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/BitStream.hpp"

// GUI
#include "Engine/API/GUI/GUILayer.hpp"
#include "Engine/API/GUI/GUIRenderer.hpp"

// Input
#include "Engine/API/Input/Input.hpp"

// Renderer
#include "Engine/API/Graphics/Buffer.hpp"
#include "Engine/API/Graphics/ComputePipeline.hpp"
#include "Engine/API/Graphics/DispatchIndirectCommand.hpp"
#include "Engine/API/Graphics/DrawIndexedIndirectCommand.hpp"
#include "Engine/API/Graphics/DrawIndirectCommand.hpp"
#include "Engine/API/Graphics/Framebuffer.hpp"
#include "Engine/API/Graphics/GraphicsPipeline.hpp"
#include "Engine/API/Graphics/Swapchain.hpp"
#include "Engine/API/Graphics/ShaderModule.hpp"
#include "Engine/Renderer/Renderer.hpp"

// Geometry (3D)
#include "Engine/API/Geometry/3D/BoxGeometry3D.hpp"

// Physics
#include "Engine/API/Physics/GJK2D.hpp"
#include "Engine/API/Physics/GJK2DConditional.hpp"
#include "Engine/API/Physics/PhysicsSimulation2D.hpp"
#include "Engine/API/Physics/SAT2D.hpp"