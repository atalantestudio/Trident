#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

namespace ProjectA {
	class OpenGLDispatchGroupsCommand : public OpenGLCommand {
		public:
			OpenGLDispatchGroupsCommand(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);

			void execute() const override;

		private:
			const GLuint num_groups_x;
			const GLuint num_groups_y;
			const GLuint num_groups_z;
	};
}