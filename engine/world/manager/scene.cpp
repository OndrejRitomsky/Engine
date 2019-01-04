#include "scene.h"

#include "../entity.h"

namespace eng {

	Scene::Scene() {}

	Scene::~Scene() {}



	void Scene::Init(core::IAllocator* allocator) {
		_entities.Init(allocator);
	}

	void Scene::AddEntity(Entity* e) {
		u32 handle = _entities.NewHandle();
		e->h = (u32) handle;
	}

	void Scene::RemoveEntity(Entity* e) {
		_entities.ReturnHandle((u32) e->h);
		e->MakeInvalid();
	}
}