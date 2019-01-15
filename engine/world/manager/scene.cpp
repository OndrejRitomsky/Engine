#include "scene.h"

#include "../entity.h"

namespace eng {

	Scene::Scene() {}

	Scene::~Scene() {}


	void Scene::Init(core::IAllocator* allocator) {
		_entities.Init(allocator);
	}

	Entity Scene::AddEntity() {
		u32 handle = _entities.NewHandle();
		return Entity::FromHash(handle);
	}

	void Scene::RemoveEntity(Entity* e) {
		_entities.ReturnHandle((u32) e->Hash());
		e->MakeInvalid();
	}
}