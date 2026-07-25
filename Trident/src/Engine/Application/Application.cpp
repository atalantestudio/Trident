// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

// TODO
#include "Engine/Adapter/Graphics/DX12/GraphicsAdapter.hpp"

#include "Engine/API/Event/Mouse/MouseDownEvent.hpp"
#include "Engine/API/Event/Mouse/MouseMoveEvent.hpp"
#include "Engine/API/Event/Mouse/MouseUpEvent.hpp"
#include "Engine/API/Event/Mouse/MouseWheelEvent.hpp"
#include "Engine/Application/Application.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	float64 Application::getFixedDeltaTime() {
		return FIXED_DELTA_TIME;
	}

	float64 Application::getInverseFixedDeltaTime() {
		return INVERSE_FIXED_DELTA_TIME;
	}

	Application::Application(const ProjectDescriptor& projectDescriptor) :
		projectType(projectDescriptor.type)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		Platform::initialize(&projectDescriptor.name[0]);

		mutex = createScope<Mutex>("ApplicationMutex");
		mutex->acquire();

		sharedMemory = createScope<SharedMemory>("ApplicationSharedMemory", sizeof(SharedMemoryData));
		sharedMemoryData = static_cast<SharedMemoryData*>(sharedMemory->map(0, sizeof(SharedMemoryData)));

		if (sharedMemoryData->instanceCount > MAX_PARALLEL_INSTANCES) {
			const sequence<char8> message = Logger::format("More than [] instances running in parallel.", static_cast<sint32>(MAX_PARALLEL_INSTANCES));

			ENGINE_ERROR(message);

			Platform::fatalExit(EOVERFLOW, message);

			return;
		}

		initializePropertyFile();
		initializeWindow(projectDescriptor);

		sharedMemoryData->instanceCount += 1;

		mutex->release();

		initializeGraphicsAdapter(projectDescriptor);

		if (projectType == ProjectType::MAIN) {
			assetLibrary = new AssetLibrary(Filesystem::getProjectRootDirectory());
			assetLibrary->loadAssetMap();
		}
	}

	void Application::initializePropertyFile() {
		// Open/create the project's property file.
		// A project shouldn't be forced to use a window, so the property file is loaded first.
		// TODO: When a setting is changed in the game menu, the property file is replaced with an updated version.
		// TODO: When the user edits the property file at runtime, the application verifies that the new values are valid before using them.
		//       If a property is invalid, the application emits a warning, but continues to use the current value.
		const std::filesystem::path projectRootDirectory = Filesystem::getProjectRootDirectory();

		const sequence<char8> propertyFilePath = Logger::format("[]/[].tpr", projectRootDirectory.string(), projectRootDirectory.filename().string());

		// Try to open the property file located at "[PROJECT_ROOT_DIRECTORY]/[PROJECT_NAME].tpr".
		Reader propertyFileReader = Filesystem::openReader(propertyFilePath);

		if (!propertyFileReader.fileExists()) {
			ENGINE_WARNING("Could not find property file \"[]\".", propertyFilePath);

			// If the property file cannot be found, create a new one with default values.
			Writer propertyFileWriter = Filesystem::openWriter(propertyFilePath);

			const view<char8> defaultFileData = PropertyFile::getDefaultFileData();

			propertyFileWriter.write(pointer_cast<const uint8*>(&defaultFileData[0]), defaultFileData.count());

			ENGINE_INFO("Created property file \"[]\" with default values.", propertyFilePath);
		}

		properties = new ApplicationProperties();
		propertyFileReader = Filesystem::openReader(propertyFilePath);

		ENGINE_ASSERT(propertyFileReader.fileExists(), "Could not find property file \"[]\".", propertyFilePath);

		propertyFileReader.seekg(std::ios::end);

		const index propertyFileSize = propertyFileReader.tellg();

		propertyFileReader.seekg(std::ios::beg);

		const sequence<uint8> propertyFileData = propertyFileReader.read(propertyFileSize);
		const view<char8> propertyFileDataView(pointer_cast<const char8*>(&propertyFileData[0]), propertyFileData.count());

		propertyFile = new PropertyFile(propertyFileDataView);
		propertyFile->readProperties(properties);

		/* TODO(property-file/graphics-api): Implement.
		Define sint32 selectedApiIndex
		Get "GraphicsAPI" setting from file
			If no setting
				Set selectedApiIndex = default API for this platform
			Else if the setting is unknown
				Emit warn "Unknown graphics API '[]'. Using API []. Updated the property in the file to avoid future warnings."
				Set selectedApiIndex = default API for this platform
				Overwrite file setting with selectedApiIndex
			Else
				Set selectedApiIndex = setting
		*/
	}

	void Application::initializeWindow(const ProjectDescriptor& projectDescriptor) {
		sequence<char8> name;

		if (sharedMemoryData->instanceCount > 0) {
			name = Logger::format("[] ([])", projectDescriptor.name, static_cast<uint32>(sharedMemoryData->instanceCount));
		}
		else {
			name = projectDescriptor.name;
		}

		window = new Window({
			.name = name,
			.extent = {properties->Window.Width, properties->Window.Height},
			.maximized = properties->Window.Maximized,
			.keyDownEventListener = BIND(Application::propagateEventFrontToBack),
			.keyUpEventListener = BIND(Application::propagateEventFrontToBack),
			.mouseDownEventListener = BIND(Application::propagateEventFrontToBack),
			.mouseMoveEventListener = BIND(Application::propagateEventFrontToBack),
			.mouseUpEventListener = BIND(Application::propagateEventFrontToBack),
			.mouseWheelEventListener = BIND(Application::propagateEventFrontToBack),
			.windowCloseEventListener = BIND(Application::onWindowClose),
			.windowResizeEventListener = BIND(Application::onWindowResize),
		});
	}

	void Application::initializeGraphicsAdapter(const ProjectDescriptor& projectDescriptor) {
		graphicsAdapter = new GraphicsAdapter(*properties);
		layerCompositor = new LayerCompositor(window, graphicsAdapter, projectDescriptor, *properties);

		Renderer::initialize(layerCompositor);
	}

	void Application::initializeLayers() {
		layerCompositor->initializeLayers();
	}

	// TODO(application/destruction-leaks)
	Application::~Application() {
		release();

		delete propertyFile;

		delete layerCompositor;

		if (lastLayerInput.swapchainImageView != nullptr) {
			lastLayerInput.swapchainImageView.~shared_ptr();
		}

		if (lastLayerInput.swapchainImage != nullptr) {
			lastLayerInput.swapchainImage.~shared_ptr();
		}

		delete graphicsAdapter;

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

	PropertyFile* Application::getPropertyFile() {
		return propertyFile;
	}

	bool Application::hasLayers() const {
		return layerCompositor->layers.size() > 0;
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

	AssetLibrary* Application::getAssetLibrary() const {
		return assetLibrary;
	}

	void Application::postConstruct() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		for (const ref<Layer>& layer : layerCompositor->layers) {
			layer->input = &lastLayerInput;
			layer->postConstruct();
		}
	}

	sint32 Application::runProcesses() const {
		const view<void*> arguments = consoleArguments;

		sint32 code = 0;

		for (const ref<Process>& process : processes) {
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
		frameTimeAccumulator += frameTime;
		previousTime = currentTime;

		uint32 updateIndex = 0;

		while (frameTimeAccumulator >= FIXED_DELTA_TIME) {
			if (updateIndex < MAX_UPDATES_PER_FRAME) {
				update(FIXED_DELTA_TIME, INVERSE_FIXED_DELTA_TIME);
			}

			// Extract 1x FIXED_DELTA_TIME from the accumulator. What's left is passed on to the next frame.
			frameTimeAccumulator -= FIXED_DELTA_TIME;

			updateIndex += 1;
		}

		if (window->isMinimized()) {
			return;
		}

		// TODO
		// Interpolate between current time and frameTimeAccumulator.
		// Use frameTimeAccumulator / FIXED_DELTA_TIME to get a value between [0, 1[.
		render(frameTimeAccumulator);

		frameIndex += 1;
		frameInFlightIndex = frameIndex % MAX_FRAMES_IN_FLIGHT;
	}

	void Application::update(float64 deltaTime, float64 inverseDeltaTime) {
		PROFILE_SCOPE("Application update/integrate");

		for (const ref<Layer>& layer : layerCompositor->layers) {
			layer->update(static_cast<float32>(deltaTime), static_cast<float32>(inverseDeltaTime));
		}
	}

	void Application::render(float64 remainingFrameTime) {
		const PresentStatus presentStatus = layerCompositor->render(frameInFlightIndex, remainingFrameTime);

		if (presentStatus == PresentStatus::ERROR_OUT_OF_DATE || presentStatus == PresentStatus::SUBOPTIMAL || windowResized) {
			resize();

			return;
		}

		ENGINE_ASSERT(presentStatus == PresentStatus::SUCCESS, "Unhandled present status.");
	}

	void Application::resize() {
		windowResized = false;

		layerCompositor->resize();

		lastLayerInput.swapchainImage = nullptr;
		lastLayerInput.swapchainImageView = nullptr;

		const Extent2D extent = window->getExtent();

		WindowResizeEvent event(extent.width, extent.height);

		// Propagate the event to the layer stack.
		propagateEventBackToFront(event);
	}

	void Application::close() {
		looping = false;
	}

	void Application::propagateEventFrontToBack(Event& event) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		for (std::vector<ref<Layer>>::const_reverse_iterator layerIterator = layerCompositor->layers.rbegin(); layerIterator != layerCompositor->layers.rend(); layerIterator += 1) {
			if (event.handled) {
				break;
			}

			(*layerIterator)->onEvent(event);
		}
	}

	void Application::propagateEventBackToFront(Event& event) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		for (std::vector<ref<Layer>>::const_iterator layerIterator = layerCompositor->layers.begin(); layerIterator != layerCompositor->layers.end(); layerIterator += 1) {
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