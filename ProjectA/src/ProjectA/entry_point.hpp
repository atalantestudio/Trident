#pragma once

#ifdef WINDOWS
	extern ProjectA::Application* ProjectA::createApplication();

	int main() {
		ProjectA::Application* application = ProjectA::createApplication();

		application->run();

		delete application;
	}
#endif