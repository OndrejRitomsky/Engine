#pragma once

#include "script_register.h"
#include "../world/entity.h"

namespace eng {
	struct Engine;
	class Transform2DComponentManager;

	class ScriptTest {
	public:
		DEFINE_SCRIPT(ScriptTest);

		ScriptTest(Engine* engine, Entity entity);
		~ScriptTest();

		void Update();

	private:
		Entity _entity;
		Transform2DComponentManager* _components;
	};
}