#include "pch.hpp"

#include "Engine/API/Event/Mouse/MouseDownEvent.hpp"
#include "Engine/API/Event/Mouse/MouseMoveEvent.hpp"
#include "Engine/API/Event/Mouse/MouseUpEvent.hpp"
#include "Engine/API/Event/Mouse/MouseWheelEvent.hpp"
#include "Engine/Application.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Platform.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	Application* Application::instance = nullptr;

	Application& Application::get() {
		return *instance;
	}

	std::string Application::getWindowName(std::string_view projectName, uint8 instanceCount) {
		if (instanceCount == 0) {
			return std::string(projectName);
		}

		return Logger::format("[] ([])", projectName, static_cast<sint32>(instanceCount));
	}

	Application::Application(const ApplicationManifest& applicationManifest) {
		PROFILE_SCOPE(FUNC_SIG);

		// TODO: Remove singleton pattern.
		DEBUG_ASSERT(instance == nullptr);

		instance = this;

		ENGINE_TRACE("Launching.");

		mutex = createScope<Mutex>("Mutex");
		mutex->acquire();

		sharedMemory = createScope<SharedMemory>("SharedMemory", sizeof(SharedMemoryData));
		sharedMemoryData = static_cast<SharedMemoryData*>(sharedMemory->map(0, sizeof(SharedMemoryData)));

		constexpr uint8 MAX_PARALLEL_INSTANCES = 5;

		Platform::setWindowName(applicationManifest.projectName.data());

		if (sharedMemoryData->instanceCount > MAX_PARALLEL_INSTANCES) {
			ENGINE_FATAL(EOVERFLOW, "More than [] instances running in parallel.", MAX_PARALLEL_INSTANCES);
		}

		// Open/create the project property file.
		// A project shouldn't be forced to use a window, so the property file is loaded first.
		// When a setting is changed in the game menu, the property file is replaced with an updated version.
		const std::filesystem::path currentPath = Filesystem::getCurrentPath();
		const std::string currentDirectoryName = Filesystem::getCurrentDirectory().string();

		std::filesystem::path propertyFilePath = currentPath / currentDirectoryName;
		propertyFilePath += ".tpr";

		ENGINE_TRACE("Opening property file.");

		propertyFile = Filesystem::load<PropertyFile*>(propertyFilePath.string());

		if (propertyFile == nullptr) {
			ENGINE_INFO("Could not find property file. Creating a new one.");

			// TODO: Recreate property file.
			DEBUG_ASSERT(false);

			return;
		}

		WindowDescriptor windowDescriptor{};
		windowDescriptor.name = getWindowName(applicationManifest.projectName, sharedMemoryData->instanceCount);
		windowDescriptor.width = propertyFile->getProperty<uint32>("ExtentWidth");
		windowDescriptor.height = propertyFile->getProperty<uint32>("ExtentHeight");
		windowDescriptor.mouseDownEventListener = BIND(Application::onEvent);
		windowDescriptor.mouseMoveEventListener = BIND(Application::onEvent);
		windowDescriptor.mouseUpEventListener = BIND(Application::onEvent);
		windowDescriptor.mouseWheelEventListener = BIND(Application::onEvent);
		windowDescriptor.windowCloseEventListener = BIND(Application::onWindowClose);
		windowDescriptor.windowResizeEventListener = BIND(Application::onWindowResize);

		sharedMemoryData->instanceCount += 1;

		ENGINE_TRACE("Running new instance. Updated count = [].", static_cast<sint32>(sharedMemoryData->instanceCount));

		mutex->release();

		window = new Window(windowDescriptor);

		const GraphicsAPI graphicsApi = propertyFile->getProperty<GraphicsAPI>("GraphicsAPI");

		Renderer::build(*window, graphicsApi);

		/* Constants */

		// API_QUEUE = Graphics API listed by priority.
		//             First APIs are more recent but more performant,
		//             last APIs are older but have better support

		/* Startup */

		// Open property file at "[PROJECT_ROOT_DIRECTORY]/[PROJECT_NAME].tpr"
			// If no file
				// Create a new property file with default values

		// Define int selectedApiIndex

		// Get "GraphicsAPI" setting (unsigned byte) from file
			// If no setting
				// Set selectedApiIndex = 0
			// Else if the setting is not in API_QUEUE
				// Emit warn "Unsupported graphics API. Using API 0. Updated the property in the file to avoid future warnings."
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
			// Emit warn "The selected API is not supported."
			// Set finalApiIndex = 0

		// Emit info "Using graphics adapter for []."
		// Define selectedApi = API_QUEUE[supportedApiIndices[finalApiIndex]]

		// Initialize selectedApi

		/*sint32 apiIndex = 0;
		sint32 apiTestQueue[5] = {
			77, // Vulkan
			63, // DirectX
			48, // Metal
			22, // WebGPU
			19, // OpenGL
		};

		{
			const auto api = apiTestQueue[apiIndex];

			for (sint32 i = apiIndex - 1; i >= 0; i--) {
				apiTestQueue[i + 1] = apiTestQueue[i];
			}

			apiTestQueue[0] = api;
		}*/
	}

	Application::~Application() {
		mutex->acquire();

		sharedMemoryData->instanceCount -= 1;

		ENGINE_DEBUG("Killing instance. Updated count = [].", static_cast<sint32>(sharedMemoryData->instanceCount));

		sharedMemory->unmap(sharedMemoryData);

		delete window;
		delete propertyFile;
	}

	const Window& Application::getWindow() const {
		return *window;
	}

	float64 Application::getCurrentFrameTime() const {
		return frameTime;
	}

	float64 Application::getFramesPerSecond() const {
		if (frameTime == 0.0) {
			return 0.0;
		}

		const float64 rendersPerSecond = 1000.0 / frameTime;

		return rendersPerSecond / 1000.0;
	}

	uint32 Application::getFrameInFlightIndex() const {
		return frameInFlightIndex;
	}

	void Application::loop() {
		window->show();

		previousTime = timer.getCurrentTime();

		while (looping) {
			window->processMessages();

			processFrame();
		}

		const ref<Device> device = Renderer::getDevice();

		device->waitIdle();
	}

	void Application::processFrame() {
		PROFILE_SCOPE(FUNC_SIG);

		const float64 currentTime = timer.getCurrentTime();

		frameTime = currentTime - previousTime;
		previousTime = currentTime;
		frameTimeAccumulator += frameTime;

		/*while (frameTimeAccumulator >= fixedDeltaTime) {
			update(fixedDeltaTime);

			// Extract the fixed delta time from the accumulator.
			// What's left is passed on to the next frame.
			frameTimeAccumulator -= fixedDeltaTime;
		}*/

		if (window->isMinimized()) {
			return;
		}

		preRender();

		// TODO: Interpolate between current time and frameTimeAccumulator.
		// See https://gafferongames.com/post/fix_your_timestep.
		render(frameTimeAccumulator);

		postRender();

		ENGINE_DEBUG("FrameIndex = []", frameIndex);

		frameIndex += 1;
		frameInFlightIndex = (frameInFlightIndex + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void Application::update(float64 fixedDeltaTime) {
		PROFILE_SCOPE("Application update/integrate");

		for (const ref<Layer>& layer : layerStack) {
			layer->update(fixedDeltaTime);
		}
	}

	void Application::preRender() {
		PROFILE_SCOPE("Application pre-render");

		for (const ref<Layer>& layer : layerStack) {
			layer->preRender();
		}
	}

	void Application::render(float64 remainingFrameTime) {
		PROFILE_SCOPE("Application render");

		const ref<Device> device = Renderer::getDevice();

		device->waitForFence(Renderer::getFlightFence(frameInFlightIndex));

		uint32 imageIndex;

		const AcquireNextImageStatus status = device->acquireNextImage(
			Renderer::getSwapchain(),
			Renderer::getAcquireNextImageSemaphore(frameInFlightIndex),
			imageIndex
		);

		if (status == AcquireNextImageStatus::ERROR_OUT_OF_DATE) {
			ENGINE_DEBUG("Recreating out-of-date swapchain.");

			// TODO: Test this path.
			DEBUG_ASSERT(false);

			resize();

			return;
		}

		ENGINE_ASSERT(status == AcquireNextImageStatus::SUBOPTIMAL || status == AcquireNextImageStatus::SUCCESS, "Failed to acquire the next swapchain image.");

		device->resetFence(Renderer::getFlightFence(frameInFlightIndex));

		Renderer::setImageIndex(imageIndex);

		const ref<CommandEncoder>& commandEncoder = Renderer::getCommandEncoder(frameInFlightIndex);

		commandEncoder->reset();
		commandEncoder->begin();

		for (const ref<Layer>& layer : layerStack) {
			layer->render(remainingFrameTime, commandEncoder);
		}

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getGraphicsQueue()->submit(
			commandBuffer,
			Renderer::getAcquireNextImageSemaphore(frameInFlightIndex),
			Renderer::getFinishedRenderSemaphore(frameInFlightIndex),
			Renderer::getFlightFence(frameInFlightIndex)
		);

		const PresentStatus presentStatus = device->getPresentQueue()->present(
			{Renderer::getSwapchain()},
			Renderer::getFinishedRenderSemaphore(frameInFlightIndex),
			{Renderer::getImageIndex()}
		);

		if (presentStatus == PresentStatus::ERROR_OUT_OF_DATE || presentStatus == PresentStatus::SUBOPTIMAL || windowResized) {
			ENGINE_INFO("Recreating the swapchain.");

			windowResized = false;

			resize();

			return;
		}

		ENGINE_ASSERT(presentStatus == PresentStatus::SUCCESS, "Unhandled present status.");
	}

	void Application::postRender() {
		PROFILE_SCOPE("Application post-render");

		for (const ref<Layer>& layer : layerStack) {
			layer->postRender();
		}
	}

	void Application::resize() {
		window->waitMessage();

		const ref<Device> device = Renderer::getDevice();

		device->waitIdle();

		WindowResizeEvent event(window->getSize()[0], window->getSize()[1]);

		SwapchainDescriptor swapchainDescriptor;
		swapchainDescriptor.extentWidth = event.getWidth();
		swapchainDescriptor.extentHeight = event.getHeight();

		Renderer::getSwapchain()->update(swapchainDescriptor);

		// Propagate the event to the layer stack.
		onEvent(event);
	}

	void Application::close() {
		looping = false;
	}

	void Application::onEvent(Event& event) {
		PROFILE_SCOPE(FUNC_SIG);

		for (std::vector<ref<Layer>>::const_reverse_iterator i = layerStack.rbegin(); i != layerStack.rend(); i += 1) {
			if (event.handled) {
				break;
			}

			(*i)->onEvent(event);
		}
	}

	void Application::onWindowResize(WindowResizeEvent& event) {
		PROFILE_SCOPE(FUNC_SIG);

		if (window->isMinimized()) {
			return;
		}

		windowResized = true;
	}

	void Application::onWindowClose(WindowCloseEvent& event) {
		close();
	}
}