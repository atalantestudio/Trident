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

	PropertyFile Application::loadPropertyFile() {
		const std::filesystem::path currentPath = Filesystem::getCurrentPath();
		const std::string currentDirectoryName = Filesystem::getCurrentDirectory().string();

		std::filesystem::path propertyFilePath = currentPath / currentDirectoryName;
		propertyFilePath += ".tpr";

		return Filesystem::load<PropertyFile>(propertyFilePath.string());
	}

	std::string Application::getWindowName(std::string_view projectName, uint8 instanceCount) {
		if (instanceCount == 0) {
			return std::string(projectName);
		}

		return Logger::format("[] ([])", projectName, static_cast<sint32>(instanceCount));
	}

	Application::Application(const ApplicationDescriptor& applicationDescriptor) {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_ASSERT(instance == nullptr, "Current instance already set.");

		instance = this;

		mutex = createScope<Mutex>("Mutex");
		mutex->acquire();

		sharedMemory = createScope<SharedMemory>("SharedMemory", sizeof(SharedMemoryData));

		sharedMemoryData = static_cast<SharedMemoryData*>(sharedMemory->map(0, sizeof(SharedMemoryData)));

		constexpr uint8 MAX_PARALLEL_INSTANCES = 5;

		Platform::setWindowName(applicationDescriptor.projectName.data());

		if (sharedMemoryData->instanceCount > MAX_PARALLEL_INSTANCES) {
			ENGINE_FATAL(EOVERFLOW, "More than [] instances running in parallel.", MAX_PARALLEL_INSTANCES);
		}

		PropertyFile propertyFile = loadPropertyFile();

		WindowDescriptor windowDescriptor{};
		windowDescriptor.name = getWindowName(applicationDescriptor.projectName, sharedMemoryData->instanceCount);
		windowDescriptor.width = propertyFile.getProperty<uint32>("ExtentWidth");
		windowDescriptor.height = propertyFile.getProperty<uint32>("ExtentHeight");

		sharedMemoryData->instanceCount += 1;

		ENGINE_TRACE("Running new instance. Updated count = [].", static_cast<sint32>(sharedMemoryData->instanceCount));

		mutex->release();

		window = createScope<Window>(windowDescriptor);
		window->setEventListener<MouseDownEvent>(BIND(Application::onEvent));
		window->setEventListener<MouseMoveEvent>(BIND(Application::onEvent));
		window->setEventListener<MouseUpEvent>(BIND(Application::onEvent));
		window->setEventListener<MouseWheelEvent>(BIND(Application::onEvent));
		window->setEventListener<WindowResizeEvent>(BIND(Application::onWindowResize));
		window->setEventListener<WindowCloseEvent>(BIND(Application::onWindowClose));

		const GraphicsAPI graphicsApi = propertyFile.getProperty<GraphicsAPI>("GraphicsAPI");

		Renderer::build(*window, graphicsApi);

		/* Constants */

		// DEFAULT_PROPERTY_FILE_PATH = Property file path used by default
		// API_QUEUE = Graphics API listed by priority.
		//             First APIs are more recent but more performant,
		//             last APIs are older but have better support

		/* Startup */

		// Open property file at DEFAULT_PROPERTY_FILE_PATH
			// If no file
				// Emit fatal
				// Return

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
		mutex->acquire();

		sharedMemoryData->instanceCount -= 1;

		ENGINE_DEBUG("Killing instance. Updated count = [].", static_cast<sint32>(sharedMemoryData->instanceCount));

		sharedMemory->unmap(sharedMemoryData);
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
		previousTime = timer.getCurrentTime();

		while (looping) {
			processFrame();
		}
	}

	void Application::processFrame() {
		PROFILE_SCOPE(FUNC_SIG);

		window->processMessages();

		const float64 currentTime = timer.getCurrentTime();

		frameTime = currentTime - previousTime;
		previousTime = currentTime;
		frameTimeAccumulator += frameTime;

		while (frameTimeAccumulator >= fixedDeltaTime) {
			update(fixedDeltaTime);

			// Extract the fixed delta time from the accumulator.
			// What's left is passed on to the next frame.
			frameTimeAccumulator -= fixedDeltaTime;
		}

		if (window->isMinimized()) {
			return;
		}

		preRender();

		// TODO: Interpolate between current time and frameTimeAccumulator.
		// See https://gafferongames.com/post/fix_your_timestep.
		render(frameTimeAccumulator);

		postRender();

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
		const ref<CommandEncoder>& commandEncoder = Renderer::getCommandEncoder();

		commandEncoder->reset();
		commandEncoder->begin();

		for (const ref<Layer>& layer : layerStack) {
			layer->render(remainingFrameTime, commandEncoder);
		}

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		device->getGraphicsQueue()->submit(commandBuffer, Renderer::getAvailableImageSemaphore(frameInFlightIndex), Renderer::getFinishedRenderSemaphore(frameInFlightIndex), Renderer::getFlightFence(frameInFlightIndex));

		const Status presentStatus = device->getPresentQueue()->present(presentData.swapchains, Renderer::getFinishedRenderSemaphore(frameInFlightIndex), presentData.imageIndices);

		resetPresentData();

		if (presentStatus == Status::SUBOPTIMAL || presentStatus == Status::ERROR_OUT_OF_DATE) {
			ENGINE_ERROR("The present status is either suboptimal or out of date.");

			// TODO: Fix resize.
			//resize();

			return;
		}

		ENGINE_ASSERT(presentStatus == Status::SUCCESS, "Unhandled present status.");

		device->waitIdle();
	}

	void Application::postRender() {
		PROFILE_SCOPE("Application post-render");

		for (const ref<Layer>& layer : layerStack) {
			layer->postRender();
		}
	}

	void Application::close() {
		looping = false;
	}

	void Application::registerPresentSubdata(const ref<Swapchain>& swapchain, uint32 imageIndex) {
		presentData.swapchains.emplace_back(swapchain);
		presentData.imageIndices.emplace_back(imageIndex);
	}

	void Application::resetPresentData() {
		presentData.swapchains.clear();
		presentData.imageIndices.clear();
	}

	void Application::onEvent(Event& event) {
		PROFILE_SCOPE(FUNC_SIG);

		for (chain<ref<Layer>>::const_reverse_iterator i = layerStack.rbegin(); i != layerStack.rend(); i += 1) {
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

		// Propagate the event to the layer stack.
		onEvent(event);
	}

	void Application::onWindowClose(WindowCloseEvent& event) {
		close();
	}
}