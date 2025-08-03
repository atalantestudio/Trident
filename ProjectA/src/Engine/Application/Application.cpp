#include "pch.hpp"

#include "Engine/API/Event/Mouse/MouseDownEvent.hpp"
#include "Engine/API/Event/Mouse/MouseMoveEvent.hpp"
#include "Engine/API/Event/Mouse/MouseUpEvent.hpp"
#include "Engine/API/Event/Mouse/MouseWheelEvent.hpp"
#include "Engine/Application/Application.hpp"
#include "Engine/Filesystem/File/PropertyFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Platform.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Engine/Debug/ScopeTimer.hpp"

namespace ProjectA {
	Application* Application::instance = nullptr;
	bool Application::looping = true;
	bool Application::windowResized = false;
	float64 Application::previousTime = 0.0;
	float64 Application::frameTime = 0.0;
	float64 Application::frameTimeAccumulator = 0.0;
	uint32 Application::frameIndex = 0;
	uint32 Application::frameInFlightIndex = 0;
	ApplicationProperties* Application::properties = nullptr;

	std::string Application::formatWindowName(std::string_view projectName, uint8 instanceCount) {
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

		ENGINE_DEBUG("Launching.");

		Platform::initialize(applicationManifest.projectName.data());

		mutex = createScope<Mutex>("Mutex");
		mutex->acquire();

		sharedMemory = createScope<SharedMemory>("SharedMemory", sizeof(SharedMemoryData));
		sharedMemoryData = static_cast<SharedMemoryData*>(sharedMemory->map(0, sizeof(SharedMemoryData)));

		if (sharedMemoryData->instanceCount > MAX_PARALLEL_INSTANCES) {
			ENGINE_FATAL(EOVERFLOW, "More than [] instances running in parallel.", static_cast<sint32>(MAX_PARALLEL_INSTANCES));

			return;
		}

		// Open/create the project's property file.
		// A project shouldn't be forced to use a window, so the property file is loaded first.
		// TODO: When a setting is changed in the game menu, the property file is replaced with an updated version.
		// TODO: When the user edits the property file themselves, the application verifies that the new values are valid before using them.
		//       If a property is invalid, the application emits a warning, but continues to use the current value.
		const std::filesystem::path projectRootDirectory = Filesystem::getProjectRootDirectory();

		const std::string propertyFilePath = Logger::format("[]\\[].tpr", projectRootDirectory.string(), projectRootDirectory.filename().string());

		// Try to open the property file located at "[PROJECT_ROOT_DIRECTORY]/[PROJECT_NAME].tpr".
		Reader propertyFilePreReader = Filesystem::openReader(propertyFilePath);

		if (!propertyFilePreReader.fileExists()) {
			ENGINE_WARN("Could not find property file \"[]\".", propertyFilePath);

			// If the property file cannot be found, create a new one with default values.
			Writer propertyFileWriter = Filesystem::openWriter(propertyFilePath);

			const std::string defaultFileData = PropertyFile::getDefaultFileData();

			propertyFileWriter.write(defaultFileData.data(), defaultFileData.size());

			ENGINE_INFO("Created new property file with default values.");
		}

		properties = new ApplicationProperties();

		Reader propertyFilePostReader = Filesystem::openReader(propertyFilePath);

		PropertyFile propertyFile(propertyFilePostReader);
		propertyFile.readProperties(properties);

		WindowDescriptor windowDescriptor{};
		windowDescriptor.name = formatWindowName(applicationManifest.projectName, sharedMemoryData->instanceCount);
		windowDescriptor.size.width = properties->WindowWidth;
		windowDescriptor.size.height = properties->WindowHeight;
		windowDescriptor.mouseDownEventListener = BIND(Application::onEvent);
		windowDescriptor.mouseMoveEventListener = BIND(Application::onEvent);
		windowDescriptor.mouseUpEventListener = BIND(Application::onEvent);
		windowDescriptor.mouseWheelEventListener = BIND(Application::onEvent);
		windowDescriptor.windowCloseEventListener = BIND(Application::onWindowClose);
		windowDescriptor.windowResizeEventListener = BIND(Application::onWindowResize);

		sharedMemoryData->instanceCount += 1;

		ENGINE_DEBUG("Running new instance. Updated count = [].", static_cast<sint32>(sharedMemoryData->instanceCount));

		mutex->release();

		window = new Window(windowDescriptor);

		Renderer::initialize(*window, properties->GraphicsAPI, properties->VSync);
	}

	Application::~Application() {
		for (const ref<Process>& process : processes) {
			process.~shared_ptr();
		}

		mutex->acquire();

		sharedMemoryData->instanceCount -= 1;

		ENGINE_DEBUG("Killing instance. Updated count = [].", static_cast<sint32>(sharedMemoryData->instanceCount));

		sharedMemory->unmap(sharedMemoryData);

		delete window;
		delete properties;
	}

	const Window& Application::getWindow() {
		return *instance->window;
	}

	float64 Application::getFixedDeltaTime() {
		return FIXED_DELTA_TIME;
	}

	float64 Application::getCurrentFrameTime() {
		return frameTime;
	}

	float64 Application::getFramesPerSecond() {
		if (frameTime == 0.0) {
			return 0.0;
		}

		const float64 rendersPerSecond = 1000.0 / frameTime;

		return rendersPerSecond / 1000.0;
	}

	uint32 Application::getFrameIndex() {
		return frameIndex;
	}

	void Application::postConstruct() {
		PROFILE_SCOPE("Application post-construct");

		for (const ref<Layer>& layer : layerStack) {
			layer->postConstruct();
		}
	}

	bool Application::hasGraphicalProcesses() const {
		return layerStack.hasLayers();
	}

	const sequence<void*>& Application::getConsoleArguments() const {
		return consoleArguments;
	}

	void Application::setConsoleArguments(uint32 argumentCount, char** argumentValues) {
		consoleArguments = sequence<void*>(argumentCount);

		for (uint32 consoleArgumentIndex = 0; consoleArgumentIndex < argumentCount; consoleArgumentIndex += 1) {
			consoleArguments[consoleArgumentIndex] = static_cast<void*>(argumentValues[consoleArgumentIndex]);
		}
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

		// TODO: Cap frameTimeAccumumlator to avoid the "spiral of death" (increasing number of physics updates).

		while (frameTimeAccumulator >= FIXED_DELTA_TIME) {
			update(FIXED_DELTA_TIME, INVERSE_FIXED_DELTA_TIME);

			// Extract the fixed delta time from the accumulator.
			// What's left is passed on to the next frame.
			frameTimeAccumulator -= FIXED_DELTA_TIME;
		}

		if (window->isMinimized()) {
			return;
		}

		// TODO: Interpolate between current time and frameTimeAccumulator.
		//       Use frameTimeAccumulator / FIXED_DELTA_TIME to get a value between 0 and 1.
		render(frameTimeAccumulator);

		frameIndex += 1;
		frameInFlightIndex = (frameInFlightIndex + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void Application::update(float64 deltaTime, float64 inverseDeltaTime) {
		PROFILE_SCOPE("Application update/integrate");

		for (const ref<Layer>& layer : layerStack) {
			layer->update(static_cast<float32>(deltaTime), static_cast<float32>(inverseDeltaTime));
		}
	}

	void Application::render(float64 remainingFrameTime) {
		PROFILE_SCOPE("Application render");

		const ref<Device> device = Renderer::getDevice();

		device->waitForFence(Renderer::getFlightFence(frameInFlightIndex));

		uint32 imageIndex;

		const AcquireNextImageStatus acquireNextImageStatus = device->acquireNextImage(
			Renderer::getSwapchain(),
			Renderer::getAcquireNextImageSemaphore(frameInFlightIndex),
			imageIndex
		);

		if (acquireNextImageStatus == AcquireNextImageStatus::ERROR_OUT_OF_DATE) {
			ENGINE_DEBUG("Recreating out-of-date swapchain.");

			// TODO: Test this path.
			DEBUG_ASSERT(false);

			resize();

			return;
		}

		ENGINE_ASSERT(acquireNextImageStatus == AcquireNextImageStatus::SUBOPTIMAL || acquireNextImageStatus == AcquireNextImageStatus::SUCCESS, "Failed to acquire the next swapchain image.");

		device->resetFence(Renderer::getFlightFence(frameInFlightIndex));

		Renderer::setImageIndex(imageIndex);

		const ref<CommandEncoder>& commandEncoder = Renderer::getCommandEncoder(frameInFlightIndex);

		commandEncoder->reset();
		commandEncoder->begin();

		for (const ref<Layer>& layer : layerStack) {
			layer->preRender();
			layer->render(remainingFrameTime, commandEncoder);
			layer->postRender();
		}

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();
		const ref<Queue> graphicsQueue = device->getQueue(QueueFamilyCapability::GRAPHICS);

		graphicsQueue->submit(
			{commandBuffer},
			Renderer::getAcquireNextImageSemaphore(frameInFlightIndex), // Must be waited on before submitting.
			Renderer::getFinishedRenderSemaphore(frameInFlightIndex), // Will be signaled once the command buffer has finished executing.
			Renderer::getFlightFence(frameInFlightIndex) // Will be signaled once the command buffer has finished executing.
		);

		const PresentStatus presentStatus = graphicsQueue->present(
			{Renderer::getSwapchain()},
			Renderer::getFinishedRenderSemaphore(frameInFlightIndex), // Must be waited on before presenting.
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

	void Application::resize() {
		window->waitMessage();

		const ref<Device> device = Renderer::getDevice();

		device->waitIdle();

		WindowResizeEvent event(window->getSize()[0], window->getSize()[1]);

		SwapchainDescriptor swapchainDescriptor;
		swapchainDescriptor.extent.width = event.getWidth();
		swapchainDescriptor.extent.height = event.getHeight();
		swapchainDescriptor.vSync = properties->VSync;

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