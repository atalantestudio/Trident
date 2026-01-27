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
#include "Engine/ECS/Component/ScriptComponent.hpp"
#include "Engine/ECS/Component/SpriteComponent.hpp"
#include "Engine/ECS/Component/Transform2DComponent.hpp"
#include "Engine/ECS/Component/Transform3DComponent.hpp"
#include "Engine/ECS/ECS.hpp"

// Filesystem
#include "Engine/Filesystem/File/FBXFile.hpp"
#include "Engine/Filesystem/File/PropertyFile.hpp"
#include "Engine/Filesystem/File/TTFFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/BitStream.hpp"

// GUI
#include "Engine/API/GUI/GUILayer.hpp"
#include "Engine/API/GUI/GUIRenderer.hpp"

// Renderer
#include "Engine/API/Rendering/Buffer.hpp"
#include "Engine/API/Rendering/ComputePipeline.hpp"
#include "Engine/API/Rendering/DispatchIndirectCommand.hpp"
#include "Engine/API/Rendering/DrawIndexedIndirectCommand.hpp"
#include "Engine/API/Rendering/DrawIndirectCommand.hpp"
#include "Engine/API/Rendering/Framebuffer.hpp"
#include "Engine/API/Rendering/RenderPipeline.hpp"
#include "Engine/API/Rendering/Swapchain.hpp"
#include "Engine/API/Rendering/ShaderModule.hpp"
#include "Engine/Renderer/Renderer.hpp"

// Geometry (2D)
#include "Engine/API/Geometry/CircleGeometry2D.hpp"
#include "Engine/API/Geometry/ConvexHullGeometry2D.hpp"
#include "Engine/API/Geometry/PointGeometry2D.hpp"

// Geometry (3D)
#include "Engine/API/Geometry/3D/BoxGeometry3D.hpp"

// Material
#include "Engine/API/Material/ColorMaterial.hpp"
#include "Engine/API/Material/ColorFillStrokeMaterial.hpp"

// Physics
#include "Engine/API/Physics/RigidBody2D.hpp"
#include "Engine/API/Physics/CCD/TimeOfImpact.hpp"
#include "Engine/API/Physics/GJK2D.hpp"
#include "Engine/API/Physics/GJK2DConditional.hpp"
#include "Engine/API/Physics/PhysicsEngine2D.hpp"
#include "Engine/API/Physics/VisualizationData.hpp"