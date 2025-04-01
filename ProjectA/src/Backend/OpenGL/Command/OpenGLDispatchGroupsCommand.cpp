#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLDispatchGroupsCommand.hpp"

namespace ProjectA {
	OpenGLDispatchGroupsCommand::OpenGLDispatchGroupsCommand(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z) :
		num_groups_x(num_groups_x),
		num_groups_y(num_groups_y),
		num_groups_z(num_groups_z)
	{}

	void OpenGLDispatchGroupsCommand::execute() const {
		glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
	}
}