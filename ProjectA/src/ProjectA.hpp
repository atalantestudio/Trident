#pragma once

#include "Engine/base.hpp"

// Math
#include "lyah/lyah.hpp"

// Application
#include "Engine/Application.hpp"
#include "Engine/Input.hpp"
#include "Engine/Layer.hpp"

// Camera controller
#include "Engine/Camera/Controller/OrthographicCameraController.hpp"

// ECS
#include "Engine/ECS/ECS.hpp"

// ImGui
#include "Engine/ImGui/ImGuiLayer.hpp"

// Loader
#include "Engine/Loader/FileLoader.hpp"

// Renderer
#include "Engine/Renderer/2D/Renderer2D.hpp"
#include "Engine/Renderer/Buffer/Buffer.hpp"
#include "Engine/Renderer/Buffer/BufferUsage.hpp"
#include "Engine/Renderer/Framebuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/RenderPipeline.hpp"
#include "Engine/Renderer/ShaderModule.hpp"
#include "Engine/Renderer/Swapchain.hpp"

// TODO: Temporary (debug)
#include "Platform/Vulkan/VulkanBuffer.hpp"
#include "Platform/Vulkan/VulkanCommandBuffer.hpp"
#include "Platform/Vulkan/VulkanCommandPool.hpp"
#include "Platform/Vulkan/VulkanDevice.hpp"
#include "Platform/Vulkan/VulkanFence.hpp"
#include "Platform/Vulkan/VulkanInstance.hpp"
#include "Platform/Vulkan/VulkanPhysicalDevice.hpp"
#include "Platform/Vulkan/VulkanPipelineLayout.hpp"
#include "Platform/Vulkan/VulkanRenderPass.hpp"
#include "Platform/Vulkan/VulkanRenderPipeline.hpp"
#include "Platform/Vulkan/VulkanSemaphore.hpp"
#include "Platform/Vulkan/VulkanShaderModule.hpp"
#include "Platform/Vulkan/VulkanSurface.hpp"
#include "Platform/Vulkan/VulkanSwapchain.hpp"