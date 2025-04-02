#include "pch.hpp"

#include <glfw/glfw3.h>

#include "Engine/Application.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	Application* Application::instance = nullptr;

	Application& Application::get() {
		return *instance;
	}

	Application::Application(const std::string& name, const ConfigurationFile& configurationFile) {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_ASSERT(instance == nullptr, "An application instance already exists.");

		instance = this;

		const std::string graphicsApi = configurationFile.get<std::string>("GraphicsAPI");

		if (graphicsApi == "OpenGL") {
			Renderer::setGraphicsApi(GraphicsAPI::OPENGL);
		}
		else if (graphicsApi == "Vulkan") {
			Renderer::setGraphicsApi(GraphicsAPI::VULKAN);
		}
		else {
			ENGINE_ASSERT(false, "Unhandled graphics API \"[]\".", graphicsApi);
		}

		const uint32 extentWidth = configurationFile.get<uint32>("ExtentWidth");
		const uint32 extentHeight = configurationFile.get<uint32>("ExtentHeight");

		// Initialize GLFW and create window.
		window = Window::create(WindowData(name, extentWidth, extentHeight));
		window->setEventCallback(BIND(Application::onEvent));

		// Initialize graphics API.
		Renderer::registerWindow(static_cast<GLFWwindow*>(window->getNativeWindow()));
		Renderer::recreate();

		RenderPassDescriptor renderPassDescriptor;
		renderPassDescriptor.loadOperation = LoadOperation::CLEAR;
		renderPassDescriptor.storeOperation = StoreOperation::STORE;

		renderPass = Renderer::getBackend()->createRenderPass(renderPassDescriptor);

		SwapchainDescriptor swapchainDescriptor;
		swapchainDescriptor.renderPass = renderPass;
		swapchainDescriptor.extentWidth = extentWidth;
		swapchainDescriptor.extentHeight = extentHeight;

		swapchain = Renderer::getBackend()->createSwapchain(swapchainDescriptor);

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::AUTO;

		auto d = Renderer::getBackend()->getDevice();
		auto queue = d->getGraphicsQueue();
		commandEncoder = queue->createCommandEncoder(commandBufferDescriptor);

		for (uint32 frameIndex = 0; frameIndex < MAX_FRAMES_IN_FLIGHT; frameIndex += 1) {
			availableImageSemaphores[frameIndex] = Renderer::getBackend()->createSemaphore();
			finishedRenderSemaphores[frameIndex] = Renderer::getBackend()->createSemaphore();
			flightFences[frameIndex] = Renderer::getBackend()->createFence();
		}

		/* Constants */

		// DEFAULT_PROPERTY_FILE_PATH = Property file path used by default
		// API_QUEUE = Graphics API listed by priority.
		//             First APIs are more recent but more performant,
		//             last APIs are older but have better support

		/* Startup */

		// Define char* path;

		// If argc > 1
			// Set path = argv[0]
		// Else
			// Set path = DEFAULT_PROPERTY_FILE_PATH

		// Open config file with path
			// If no file
				// Emit fatal
				// Return

		// Define int selectedApiIndex

		// Get "graphics_api" int setting from file
			// If no setting
				// Set selectedApiIndex = 0
			// Else if the setting is not in API_QUEUE
				// Emit warn "Unknown graphics API"
				// Set selectedApiIndex = 0
				// Overwrite file setting with selectedApiIndex
			// Else
				// Set selectedApiIndex = setting

		// Define queue apiTestQueue = API_QUEUE
			// Move the API at selectedApiIndex to the top

		// Define selectedApi
		// Define supportedApiIndices

		// For each API testedApi in API_QUEUE
			// Define available = testIfAvailable(testedApi)
			// If available
				// Push index of testedApi to supportedApiIndices

		// Define finalApiIndex = -1

		// For each API supportedApi in supportedApiIndices
			// If selectedApiIndex = index of supportedApi
				// Set finalApiIndex = index of supportedApi
				// Break

		// If finalApiIndex = -1
			// Emit warn "Selected API is unsupported"
			// Set finalApiIndex = 0

		// Emit info "Using backend"
		// Define selectedApi = API_QUEUE[supportedApiIndices[finalApiIndex]]

		// Initialize selectedApi

		/*int apiIndex = 0;
		int apiTestQueue[5] = {
			77, // Vulkan
			63, // DirectX
			48, // Metal
			22, // WebGPU
			19, // OpenGL
		};

		{
			const auto api = apiTestQueue[apiIndex];

			for (int i = apiIndex - 1; i >= 0; i--) {
				apiTestQueue[i + 1] = apiTestQueue[i];
			}

			apiTestQueue[0] = api;
		}*/
	}

	Application::~Application() {
		PROFILE_SCOPE(FUNC_SIG);
	}

	Window& Application::getWindow() {
		return *window;
	}

	const ref<RenderPass>& Application::getRenderPass() const {
		return renderPass;
	}

	const ref<Swapchain>& Application::getSwapchain() const {
		return swapchain;
	}

	void Application::loop() {
		PROFILE_SCOPE(FUNC_SIG);

		while (looping) {
			PROFILE_SCOPE("Game loop");

			const float32 time = static_cast<float32>(glfwGetTime());
			const float32 deltaTime = time - lastFrameTime;

			lastFrameTime = time;

			if (!minimized) {
				render(deltaTime);
			}

			window->update();
		}
	}

	void Application::render(float32 deltaTime) {
		PROFILE_SCOPE("Update layers");

		const ref<Device>& device = Renderer::getBackend()->getDevice();

		device->waitForFence(flightFences[frameInFlightIndex]);

		uint32 nextImageIndex;

		const Status acquireNextImageStatus = device->acquireNextImage(Application::get().getSwapchain(), availableImageSemaphores[frameInFlightIndex], nextImageIndex);

		ENGINE_ASSERT(acquireNextImageStatus == Status::SUBOPTIMAL || acquireNextImageStatus == Status::SUCCESS, "Failed to acquire next image.");

		device->resetFence(flightFences[frameInFlightIndex]);

		commandEncoder->reset();
		commandEncoder->begin();

		for (Layer* layer : layerStack) {
			layer->update(deltaTime, commandEncoder, nextImageIndex);
		}

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getGraphicsQueue()->submit(commandBuffer, availableImageSemaphores[frameInFlightIndex], finishedRenderSemaphores[frameInFlightIndex], flightFences[frameInFlightIndex]);

		const Status presentStatus = device->present(Application::get().getSwapchain(), finishedRenderSemaphores[frameInFlightIndex], nextImageIndex);

		if (presentStatus == Status::SUBOPTIMAL || presentStatus == Status::ERROR_OUT_OF_DATE || needsResize) {
			resize();

			return;
		}

		ENGINE_ASSERT(presentStatus == Status::SUCCESS, "Unhandled present status.");

		device->waitIdle();

		frameInFlightIndex = (frameInFlightIndex + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void Application::resize() {
		sint32 extentWidth;
		sint32 extentHeight;

		Renderer::getWindowExtent(extentWidth, extentHeight);

		const ref<Device>& device = Renderer::getBackend()->getDevice();

		device->waitIdle();

		SwapchainDescriptor swapchainDescriptor;
		swapchainDescriptor.renderPass = Application::get().getRenderPass();
		swapchainDescriptor.extentWidth = static_cast<uint32>(extentWidth);
		swapchainDescriptor.extentHeight = static_cast<uint32>(extentHeight);

		Application::get().getSwapchain()->recreate(swapchainDescriptor);

		needsResize = false;
	}

	void Application::close() {
		looping = false;
	}

	void Application::onEvent(Event& event) {
		PROFILE_SCOPE(FUNC_SIG);

		EventDispatcher eventDispatcher(event);

		eventDispatcher.dispatch<WindowCloseEvent>(BIND(Application::onWindowClose));
		eventDispatcher.dispatch<WindowResizeEvent>(BIND(Application::onWindowResize));

		for (chain<Layer*>::const_reverse_iterator i = layerStack.rbegin(); i != layerStack.rend(); i += 1) {
			if (event.handled) {
				break;
			}

			(*i)->onEvent(event);
		}
	}

	bool Application::onWindowClose(WindowCloseEvent& event) {
		close();

		return true;
	}

	bool Application::onWindowResize(WindowResizeEvent& event) {
		PROFILE_SCOPE(FUNC_SIG);

		if (event.getWidth() == 0 || event.getHeight() == 0) {
			minimized = true;

			return false;
		}

		minimized = false;
		needsResize = true;

		return false;
	}
}