#pragma once

#include <core/algorithm/hash.h>
#include <core/algorithm/cstring.h>

#include "engine.h"
#include "file_system/file_system_event.h"
#include "file_system/file_system_module.h"

namespace eng {


	void LoadShaders(Engine* engine) {
		FileSystemModule* fsm = engine->fileSystemModule;
		ResourceModule* frm = engine->resourceModule;
		const u32 N = 4;

		const char* paths[N] = {"assets/shader/box_vs.glsl", "assets/shader/box_fs.glsl", 
			"assets/shader/textured_box_vs.glsl", "assets/shader/textured_box_fs.glsl"};
		h64 hashes[N];
		for (u32 i = 0; i < N; ++i) {
			hashes[i] = core::Hash(paths[i], (u32) core::Strlen(paths[i]));
		}

		FileSystemRegisterPathEvents evt;
		evt.paths = paths;
		evt.hashes = hashes;
		evt.count = N;

		fsm->OnEventsByType(engine->frameModule->CurrentFrame(), &evt, FileSystemEventType::REGISTER_PATH);
	}

	void LoadPrograms(Engine* engine) {

		FileSystemModule* fsm = engine->fileSystemModule;
		ResourceModule* frm = engine->resourceModule;
		const u32 N = 2;

		const char* paths[N] = {"assets/shader/box_program.json", "assets/shader/textured_box_program.json"};
		h64 hashes[N];
		u64 typeIDs[N];

		for (u32 i = 0; i < N; ++i) {
			hashes[i] = core::Hash(paths[i], (u32) core::Strlen(paths[i]));
			typeIDs[i] = (u64) ResourceType::SHADER_PROGRAM;
		}

		FileSystemRegisterPathEvents evt;
		evt.paths = paths;
		evt.hashes = hashes;
		evt.count = N;

		fsm->OnEventsByType(engine->frameModule->CurrentFrame(), &evt, FileSystemEventType::REGISTER_PATH);
	}

	void LoadTextures(Engine* engine) {
		FileSystemModule* fsm = engine->fileSystemModule;
		ResourceModule* frm = engine->resourceModule;
		const u32 N = 1;

		const char* paths[] = {"assets/texture/wall.jpg"};
		h64 hashes[N];
		for (u32 i = 0; i < N; ++i) {
			hashes[i] = core::Hash(paths[i], (u32) core::Strlen(paths[i]));
		}

		FileSystemRegisterPathEvents evt;
		evt.paths = paths;
		evt.hashes = hashes;
		evt.count = N;

		fsm->OnEventsByType(engine->frameModule->CurrentFrame(), &evt, FileSystemEventType::REGISTER_PATH);
	}

	void LoadMaterialTemplates(Engine* engine) {
		FileSystemModule* fsm = engine->fileSystemModule;
		ResourceModule* frm = engine->resourceModule;
		const u32 N = 2;

		const char* paths[N] = {"assets/material/box_material_template.json", "assets/material/textured_box_material_template.json"};
		h64 hashes[N];
		u64 typeIDs[N];

		for (u32 i = 0; i < N; ++i) {
			hashes[i] = core::Hash(paths[i], (u32) core::Strlen(paths[i]));
			typeIDs[i] = (u64) ResourceType::MATERIAL_TEMPLATE;
		}

		FileSystemRegisterPathEvents evt;
		evt.paths = paths;
		evt.hashes = hashes;
		evt.count = N;

		fsm->OnEventsByType(engine->frameModule->CurrentFrame(), &evt, FileSystemEventType::REGISTER_PATH);
	}

	void LoadMaterials(Engine* engine) {
		FileSystemModule* fsm = engine->fileSystemModule;
		ResourceModule* frm = engine->resourceModule;
		const u32 N = 2;

		const char* paths[N] = {"assets/material/box_material.json", "assets/material/textured_box_material.json"};
		h64 hashes[N];
		u64 typeIDs[N];

		for (u32 i = 0; i < N; ++i) {
			hashes[i] = core::Hash(paths[i], (u32) core::Strlen(paths[i]));
			typeIDs[i] = (u64) ResourceType::MATERIAL;
		}

		FileSystemRegisterPathEvents evt;
		evt.paths = paths;
		evt.hashes = hashes;
		evt.count = N;

		fsm->OnEventsByType(engine->frameModule->CurrentFrame(), &evt, FileSystemEventType::REGISTER_PATH);
	}

	void LoadMeshes(Engine* engine) {
		FileSystemModule* fsm = engine->fileSystemModule;
		ResourceModule* frm = engine->resourceModule;
		const u32 N = 2;

		const char* paths[N] = {"assets/mesh/box_untextured.json", "assets/mesh/box_textured.json"};
		h64 hashes[N];
		u64 typeIDs[N];

		for (u32 i = 0; i < N; ++i) {
			hashes[i] = core::Hash(paths[i], (u32) core::Strlen(paths[i]));
			typeIDs[i] = (u64) ResourceType::MESH;
		}

		FileSystemRegisterPathEvents evt;
		evt.paths = paths;
		evt.hashes = hashes;
		evt.count = N;

		fsm->OnEventsByType(engine->frameModule->CurrentFrame(), &evt, FileSystemEventType::REGISTER_PATH);
	}


	void LoadRenderObjects(Engine* engine) {
		FileSystemModule* fsm = engine->fileSystemModule;
		ResourceModule* frm = engine->resourceModule;
		const u32 N = 2;

		const char* paths[N] = {"assets/render_object/box_render_object.json", "assets/render_object/textured_box_render_object.json"};
		h64 hashes[N];
		u64 typeIDs[N];

		for (u32 i = 0; i < N; ++i) {
			hashes[i] = core::Hash(paths[i], (u32) core::Strlen(paths[i]));
			typeIDs[i] = (u64) ResourceType::RENDER_OBJECT;
		}

		FileSystemRegisterPathEvents evt;
		evt.paths = paths;
		evt.hashes = hashes;
		evt.count = N;

		fsm->OnEventsByType(engine->frameModule->CurrentFrame(), &evt, FileSystemEventType::REGISTER_PATH);
	}

}