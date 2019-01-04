#pragma once

#include "Engine/Engine.h"
#include "Engine/FileSystem/FileSystemEvent.h"

#include <core/algorithm/hash.h>
#include <core/algorithm/cstring.h>

#include "Engine/FileSystem/CFileSystemModule.h"

#include "resources/resource/material.h"
#include "resources/resource/shader_program.h"
#include "resources/resource/mesh.h"
#include "resources/resource/render_object.h"
#include "resources/resource/template/material_template.h"


namespace eng {


	u32 LoadShaders(Engine* engine) {
		CFileSystemModule* fsm = engine->fileSystemModule;
		ResourceModule* frm = engine->resourceModule;
		const u32 N = 2;

		const char* paths[N] = {"assets/shader/box_vs.glsl", "assets/shader/box_fs.glsl"};
		h64 hashes[N];
		for (u32 i = 0; i < N; ++i) {
			hashes[i] = core::Hash(paths[i], (u32) core::Strlen(paths[i]));
		}

		FileSystemRegisterPathEvents evt;
		evt.paths = paths;
		evt.hashes = hashes;
		evt.count = N;

		fsm->OnEventsByType(engine->frameModule->CurrentFrame(), &evt, FileSystemEventType::REGISTER_PATH);
		return N;
	}

	u32 LoadPrograms(Engine* engine) {

		CFileSystemModule* fsm = engine->fileSystemModule;
		ResourceModule* frm = engine->resourceModule;
		const u32 N = 1;

		const char* paths[N] = {"assets/shader/box_program.json"};
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

		//ResourcePreloadEvents e{N, typeIDs, hashes};
		//frm->OnEventsByType(engine->frameModule->CurrentFrame(), &e, ResourceEventType::PRELOAD);

		return 0;
	}

	u32 LoadTextures(Engine* engine) {
		CFileSystemModule* fsm = engine->fileSystemModule;
		ResourceModule* frm = engine->resourceModule;
		const u32 N = 1;

		const char* paths[] = {"wall.png"};
		h64 hashes[N];
		for (u32 i = 0; i < N; ++i) {
			hashes[i] = core::Hash(paths[i], (u32) core::Strlen(paths[i]));
		}

		FileSystemRegisterPathEvents evt;
		evt.paths = paths;
		evt.hashes = hashes;

		fsm->OnEventsByType(engine->frameModule->CurrentFrame(), &evt, FileSystemEventType::REGISTER_PATH);

	/*	u64 type = static_cast<u64>(ResourceType::TEXTURE);

		ResourcePreloadEvents e{N, &type, hashes};
		frm->OnEventsByType(&e,ResourceEventType::PRELOAD);*/

		return 1;
	}

	u32 LoadMaterialTemplates(Engine* engine) {
		CFileSystemModule* fsm = engine->fileSystemModule;
		ResourceModule* frm = engine->resourceModule;
		const u32 N = 1;

		const char* paths[N] = {"assets/material/box_material_template.json"};
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

		//ResourcePreloadEvents e{N, typeIDs, hashes};
		//frm->OnEventsByType(engine->frameModule->CurrentFrame(), &e, ResourceEventType::PRELOAD);

		return 0;
	}

	u32 LoadMaterials(Engine* engine) {
		CFileSystemModule* fsm = engine->fileSystemModule;
		ResourceModule* frm = engine->resourceModule;
		const u32 N = 1;

		const char* paths[N] = {"assets/material/box_material.json"};
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

		//ResourcePreloadEvents e{N, typeIDs, hashes};
		//frm->OnEventsByType(engine->frameModule->CurrentFrame(), &e, ResourceEventType::PRELOAD);
		return 0;
	}

	u32 LoadMeshes(Engine* engine) {
		/*ResourceModule* frm = engine->resourceModule;
		const u32 N = 1;

		f32 vertices[] = {-0.5, -0.5, 0.0f, 0.0f,
											0.5, -0.5, 1.0f, 0.0f,
											0.5,  0.5, 1.0f, 1.0f,
											-0.5,  0.5, 0.0f, 1.0f};

		u32 indices[] = {0, 1, 2,
										 2, 3, 0};

		MeshDescription mesh;
		mesh.vertexBuffer.buffer = vertices;
		mesh.vertexBuffer.bufferSize = sizeof(vertices);
		mesh.indexBuffer.buffer = indices;
		mesh.indexBuffer.bufferSize = sizeof(indices);
		mesh.hash = core::Hash("mesh1", 6);


		u64 type = static_cast<u64>(ResourceType::MESH);
		char* data = reinterpret_cast<char*>(&mesh);
		u64 sizes = 0;
		h64 h = core::Hash("mesh1", 6);

		ResourceRegisterEvents evt;
		evt.typeIDs = &type;
		evt.buffers = &data;
		evt.bufferSizes = &sizes;
		evt.hashes = &h;
		evt.count = 1;*/
		//frm->OnEventsByType(&evt, N, ResourceEventType::REGISTER);

		CFileSystemModule* fsm = engine->fileSystemModule;
		ResourceModule* frm = engine->resourceModule;
		const u32 N = 1;

		const char* paths[N] = {"assets/mesh/box_untextured.json"};
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

	/*	ResourcePreloadEvents e{N, typeIDs, hashes};
		frm->OnEventsByType(engine->frameModule->CurrentFrame(), &e, ResourceEventType::PRELOAD);*/

		return 0;
	}


	u32 LoadRenderObjects(Engine* engine) {
		CFileSystemModule* fsm = engine->fileSystemModule;
		ResourceModule* frm = engine->resourceModule;
		const u32 N = 1;

		const char* paths[N] = {"assets/render_object/box_render_object.json"};
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

		//ResourcePreloadEvents e{N, typeIDs, hashes};
		//frm->OnEventsByType(engine->frameModule->CurrentFrame(), &e, ResourceEventType::PRELOAD);

		return 0;
	}


	

}