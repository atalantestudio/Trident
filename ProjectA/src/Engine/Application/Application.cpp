#include "pch.hpp"

#include "Engine/API/Event/Mouse/MouseDownEvent.hpp"
#include "Engine/API/Event/Mouse/MouseMoveEvent.hpp"
#include "Engine/API/Event/Mouse/MouseUpEvent.hpp"
#include "Engine/API/Event/Mouse/MouseWheelEvent.hpp"
#include "Engine/Application/Application.hpp"
#include "Engine/Filesystem/File/PropertyFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
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

			const std::string defaultFileData = PropertyFile::getDefaultFileData();

			sequence<uint8> data(defaultFileData.size());

			std::copy(defaultFileData.begin(), defaultFileData.end(), &data[0]);

			propertyFileWriter.write(&data[0], data.count());

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
		windowDescriptor.keyDownEventListener = BIND(Application::onEvent);
		windowDescriptor.keyUpEventListener = BIND(Application::onEvent);
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

		Renderer::initialize(projectDescriptor, *window, properties->GraphicsAPI, properties->VSync);

		if (projectType == ProjectType::MAIN) {
			ENGINE_INFO("Loading assets.");

			assetLibrary = new AssetLibrary();
			assetLibrary->loadAssets();
		}
	}

	Application::~Application() {
		if (projectType == ProjectType::MAIN) {
			delete assetLibrary;
		}

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

		for (const ref<Layer>& layer : layerStack) {
			layer->postConstruct();
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

		// TODO: Interpolate between current time and frameTimeAccumulator
		//       (use frameTimeAccumulator / FIXED_DELTA_TIME to get a value between 0 and 1).
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
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		for (std::vector<ref<Layer>>::const_reverse_iterator i = layerStack.rbegin(); i != layerStack.rend(); i += 1) {
			if (event.handled) {
				break;
			}

			(*i)->onEvent(event);
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