// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Event/Mouse/MouseDownEvent.hpp"
#include "Engine/API/Event/Mouse/MouseMoveEvent.hpp"
#include "Engine/API/Event/Mouse/MouseUpEvent.hpp"
#include "Engine/API/Event/Mouse/MouseWheelEvent.hpp"
#include "Engine/Application/Application.hpp"
#include "Engine/Filesystem/File/PropertyFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	float64 Application::getFixedDeltaTime() {
		return FIXED_DELTA_TIME;
	}

	float64 Application::getInverseFixedDeltaTime() {
		return INVERSE_FIXED_DELTA_TIME;
	}

	sequence<char8> Application::formatWindowName(view<char8> projectName, uint8 instanceCount) {
		if (instanceCount == 0) {
			return projectName;
		}

		return Logger::format("[] ([])", projectName, static_cast<sint32>(instanceCount));
	}

	Application::Application(const ProjectDescriptor& projectDescriptor) :
		projectType(projectDescriptor.type)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ENGINE_DEBUG("Launching.");

		Platform::initialize(&projectDescriptor.name[0]);

		mutex = createScope<Mutex>("Mutex");
		mutex->acquire();

		sharedMemory = createScope<SharedMemory>("SharedMemory", sizeof(SharedMemoryData));
		sharedMemoryData = static_cast<SharedMemoryData*>(sharedMemory->map(0, sizeof(SharedMemoryData)));

		if (sharedMemoryData->instanceCount > MAX_PARALLEL_INSTANCES) {
			const sequence<char8> message = Logger::format("More than [] instances running in parallel.", static_cast<sint32>(MAX_PARALLEL_INSTANCES));

			ENGINE_ERROR(message);

			Platform::fatalExit(EOVERFLOW, message);

			return;
		}

		// Open/create the project's property file.
		// A project shouldn't be forced to use a window, so the property file is loaded first.
		// TODO: When a setting is changed in the game menu, the property file is replaced with an updated version.
		// TODO: When the user edits the property file at runtime, the application verifies that the new values are valid before using them.
		//       If a property is invalid, the application emits a warning, but continues to use the current value.
		const std::filesystem::path projectRootDirectory = Filesystem::getProjectRootDirectory();

		const sequence<char8> propertyFilePath = Logger::format("[]\\[].tpr", projectRootDirectory.string(), projectRootDirectory.filename().string());

		// Try to open the property file located at "[PROJECT_ROOT_DIRECTORY]/[PROJECT_NAME].tpr".
		Reader propertyFileReader = Filesystem::openReader(propertyFilePath);

		if (!propertyFileReader.fileExists()) {
			ENGINE_WARNING("Could not find property file \"[]\".", propertyFilePath);

			// If the property file cannot be found, create a new one with default values.
			Writer propertyFileWriter = Filesystem::openWriter(propertyFilePath);

			const view<char8> defaultFileData = PropertyFile::getDefaultFileData();

			propertyFileWriter.write(pointer_cast<const uint8*>(&defaultFileData[0]), defaultFileData.count());

			ENGINE_INFO("Created new property file with default values.");
		}

		properties = new ApplicationProperties();
		propertyFileReader = Filesystem::openReader(propertyFilePath);

		// TODO: Detect if the properties are invalid due to the file not being found.
		PropertyFile propertyFile(propertyFileReader);
		propertyFile.readProperties(properties);

		WindowDescriptor windowDescriptor{};
		windowDescriptor.name = formatWindowName(projectDescriptor.name, sharedMemoryData->instanceCount);
		windowDescriptor.size.width = properties->WindowWidth;
		windowDescriptor.size.height = properties->WindowHeight;
		windowDescriptor.keyDownEventListener = BIND(Application::propagateEventFrontToBack);
		windowDescriptor.keyUpEventListener = BIND(Application::propagateEventFrontToBack);
		windowDescriptor.mouseDownEventListener = BIND(Application::propagateEventFrontToBack);
		windowDescriptor.mouseMoveEventListener = BIND(Application::propagateEventFrontToBack);
		windowDescriptor.mouseUpEventListener = BIND(Application::propagateEventFrontToBack);
		windowDescriptor.mouseWheelEventListener = BIND(Application::propagateEventFrontToBack);
		windowDescriptor.windowCloseEventListener = BIND(Application::onWindowClose);
		windowDescriptor.windowResizeEventListener = BIND(Application::onWindowResize);

		sharedMemoryData->instanceCount += 1;

		mutex->release();

		window = new Window(windowDescriptor);

		Renderer::initialize(projectDescriptor, *window, *properties);

		if (projectType == ProjectType::MAIN) {
			ENGINE_INFO("Loading assets.");

			assetLibrary = new AssetLibrary();
			assetLibrary->loadAssets();
		}
	}

	Application::~Application() {
		for (const ref<Layer> layer : layerStack) {
			layer.~shared_ptr();
		}

		lastLayerInput.swapchainImageView.~shared_ptr();
		lastLayerInput.swapchainImage.~shared_ptr();

		layerStack.clear();

		Renderer::deinitialize();

		mutex->acquire();

		sharedMemoryData->instanceCount -= 1;

		sharedMemory->unmap(sharedMemoryData);
	}

	const sequence<void*>& Application::getConsoleArguments() const {
		return consoleArguments;
	}

	void Application::setConsoleArguments(uint32 argumentCount, char8** argumentValues) {
		consoleArguments = sequence<void*>(argumentCount);

		for (uint32 consoleArgumentIndex = 0; consoleArgumentIndex < argumentCount; consoleArgumentIndex += 1) {
			consoleArguments[consoleArgumentIndex] = static_cast<void*>(argumentValues[consoleArgumentIndex]);
		}
	}

	bool Application::hasLayers() const {
		return layerStack.hasLayers();
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

	uint32 Application::getFrameIndex() const {
		return frameIndex;
	}

	const Window* Application::getWindow() const {
		return window;
	}

	const AssetLibrary* Application::getAssetLibrary() const {
		return assetLibrary;
	}

	void Application::postConstruct() {
		PROFILE_SCOPE("Application post-construct");

		for (const ref<Layer> layer : layerStack) {
			layer->input = &lastLayerInput;

			layer->postConstruct();
		}
	}

	sint32 Application::runProcesses() const {
		const view<void*> arguments = consoleArguments;

		sint32 code = 0;

		for (const ref<Process> process : processes) {
			code = process->main(arguments);

			if (code != 0) {
				ENGINE_DEBUG("Process [] exited with code [].", process->getId(), code);

				break;
			}
		}

		return code;
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
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const float64 currentTime = timer.getCurrentTime();

		frameTime = currentTime - previousTime;
		previousTime = currentTime;
		frameTimeAccumulator += frameTime;

		while (frameTimeAccumulator >= FIXED_DELTA_TIME) {
			update(FIXED_DELTA_TIME, INVERSE_FIXED_DELTA_TIME);

			// Extract the fixed delta time from the accumulator.
			// What's left is passed on to the next frame.
			frameTimeAccumulator -= FIXED_DELTA_TIME;
		}

		if (window->isMinimized()) {
			return;
		}

		// TODO
		// Interpolate between current time and frameTimeAccumulator.
		// Use frameTimeAccumulator / FIXED_DELTA_TIME to get a value between 0 and 1.
		render(frameTimeAccumulator);

		frameIndex += 1;
		frameInFlightIndex += 1;
		frameInFlightIndex %= MAX_FRAMES_IN_FLIGHT;
	}

	void Application::update(float64 deltaTime, float64 inverseDeltaTime) {
		PROFILE_SCOPE("Application update/integrate");

		for (const ref<Layer> layer : layerStack) {
			layer->update(static_cast<float32>(deltaTime), static_cast<float32>(inverseDeltaTime));
		}
	}

	void Application::render(float64 remainingFrameTime) {
		PROFILE_SCOPE("Application render");

		const ref<Device> device = Renderer::getDevice();
		const ref<Swapchain> swapchain = Renderer::getSwapchain();
		const ref<Fence> flightFence = Renderer::getFlightFence(frameInFlightIndex);
		const ref<Semaphore> acquireNextImageSemaphore = Renderer::getAcquireNextImageSemaphore(frameInFlightIndex);
		const ref<Semaphore> finishedRenderSemaphore = Renderer::getFinishedRenderSemaphore(frameInFlightIndex);
		const ref<CommandEncoder> commandEncoder = Renderer::getCommandEncoder(frameInFlightIndex);

		device->waitForFence(flightFence);
		device->resetFence(flightFence);

		uint32 imageIndex;

		const AcquireNextImageStatus acquireNextImageStatus = device->acquireNextImage(swapchain, acquireNextImageSemaphore, imageIndex);

		ENGINE_ASSERT(acquireNextImageStatus == AcquireNextImageStatus::SUBOPTIMAL || acquireNextImageStatus == AcquireNextImageStatus::SUCCESS, "Failed to acquire the next swapchain image.");

		Renderer::setImageIndex(imageIndex);

		commandEncoder->reset();
		commandEncoder->begin();

		for (const ref<Layer> layer : layerStack) {
			layer->preRender();
			layer->render(remainingFrameTime, commandEncoder);
			layer->postRender();
		}

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();
		const ref<Queue> graphicsQueue = device->getQueue(QueueFamilyCapability::GRAPHICS);

		graphicsQueue->submit({commandBuffer}, acquireNextImageSemaphore, finishedRenderSemaphore, flightFence);

		const PresentStatus presentStatus = graphicsQueue->present({swapchain}, {imageIndex}, finishedRenderSemaphore);

		if (presentStatus == PresentStatus::ERROR_OUT_OF_DATE || presentStatus == PresentStatus::SUBOPTIMAL || windowResized) {
			resize();

			return;
		}

		ENGINE_ASSERT(presentStatus == PresentStatus::SUCCESS, "Unhandled present status.");
	}

	// TODO: Handle resize events with layer superposition.
	void Application::resize() {
		windowResized = false;

		window->waitMessage();

		const ref<Device> device = Renderer::getDevice();

		device->waitIdle();

		WindowResizeEvent event(window->getSize()[0], window->getSize()[1]);

		SwapchainDescriptor swapchainDescriptor;
		swapchainDescriptor.extent.width = event.getWidth();
		swapchainDescriptor.extent.height = event.getHeight();
		swapchainDescriptor.vSync = properties->VerticalSync;

		Renderer::getSwapchain()->update(swapchainDescriptor);

		lastLayerInput.swapchainImage = nullptr;
		lastLayerInput.swapchainImageView = nullptr;

		// Propagate the event to the layer stack.
		propagateEventBackToFront(event);
	}

	void Application::close() {
		looping = false;
	}

	void Application::propagateEventFrontToBack(Event& event) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		for (std::vector<ref<Layer>>::const_reverse_iterator layerIterator = layerStack.rbegin(); layerIterator != layerStack.rend(); layerIterator += 1) {
			if (event.handled) {
				break;
			}

			(*layerIterator)->onEvent(event);
		}
	}

	void Application::propagateEventBackToFront(Event& event) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		for (std::vector<ref<Layer>>::const_iterator layerIterator = layerStack.begin(); layerIterator != layerStack.end(); layerIterator += 1) {
			// event.handled is ignored in back to front scenarios.
			(*layerIterator)->onEvent(event);
		}
	}

	void Application::onWindowResize(WindowResizeEvent& event) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		if (window->isMinimized()) {
			return;
		}

		windowResized = true;
	}

	void Application::onWindowClose(WindowCloseEvent& event) {
		close();
	}
}