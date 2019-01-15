#include "script_test.h"

#include "../engine.h"

#include "../world/world_module.h"
#include "../world/manager/transform2d_component_manager.h"

namespace eng {

	REGISTER_SCRIPT(ScriptTest);


	ScriptTest::ScriptTest(Engine* engine, Entity entity) {
		_entity = entity;
		_components = engine->worldModule->GetTransform2DComponentManager();
	}

	ScriptTest::~ScriptTest() {

	}

	void ScriptTest::Update() {
		auto transformHandle = _components->Find(_entity);
		f32 angle = _components->Angle(transformHandle);
		angle += 0.05f;
		if (angle > 6.29f)
			angle -= 6.29f;

		_components->SetAngle(transformHandle, angle);
	}
}