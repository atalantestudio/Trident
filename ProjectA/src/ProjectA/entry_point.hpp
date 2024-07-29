#pragma once

#ifdef WINDOWS
	extern ProjectA::Application* ProjectA::createApplication();

	int main() {
		ProjectA::Logger::initialize();

		LOG_CORE_TRACE("Initialized core logging");
		LOG_CLIENT_TRACE("Initialized client logging");

		ProjectA::Application* application = ProjectA::createApplication();
		application->run();

		delete application;
	}
#endif