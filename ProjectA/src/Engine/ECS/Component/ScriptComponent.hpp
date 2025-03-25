#pragma once

#include "Engine/ECS/ScriptEntity.hpp"

namespace ProjectA {
	struct ScriptComponent {
		ScriptEntity* entity = nullptr;

		ScriptEntity* (*constructScript)();
		void (*destroyScript)(ScriptComponent*);

		template<typename T>
		void bind() {
			constructScript = []() {
				return static_cast<ScriptEntity*>(static_cast<void*>(new T()));
			};

			destroyScript = [](ScriptComponent* scriptComponent) {
				delete scriptComponent->entity;

				scriptComponent->entity = nullptr;
			};
		}
	};
}