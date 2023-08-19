#pragma once

#ifdef WINDOWS
	extern ProjectA::Application* ProjectA::createApplication();

	int main() {
		ProjectA::Logger::initialize();
		LOG_CORE_INFO("Initialized core logging");
		LOG_CLIENT_INFO("Initialized client logging");

		ProjectA::Application* application = ProjectA::createApplication();
		application->run();

		delete application;
	}
#endif