#pragma once

#include "Engine/Engine.h"
#include "Engine/FileSystem/FileSystemEvent.h"

#include <Core/Algorithm/Hash.h>

#include "Engine/FileSystem/CFileSystemModule.h"

#include "Engine/Resources/Resource/Material.h"
#include "Engine/Resources/Resource/ShaderProgram.h"
#include "Engine/Resources/Resource/Mesh.h"
#include "Engine/Resources/Resource/RenderObject.h"
#include "Engine/Resources/Resource/Template/CMaterialTemplate.h"

namespace eng {


	u32 LoadShaders(Engine* engine) {
		CFileSystemModule* fsm = engine->fileSystemModule;
		CResourceModule* frm = engine->resourceModule;
		const u32 N = 2;

		const char* paths[N] = {"Shaders/simpleLineVertex.glsl", "Shaders/simpleLineFragment.glsl"};
		h64 hashes[N];
		for (u32 i = 0; i < N; ++i) {
			hashes[i] = core::ToHash(paths[i], (u32) strlen(paths[i]));
		}

		FileSystemRegisterPathEvents evt;
		evt.paths = paths;
		evt.hashes = hashes;
		evt.count = N;
	
		fsm->OnEventsByType(&evt, FileSystemEventType::REGISTER_PATH);

		/*
		h64 hashes[N];
		for (u32 i = 0; i < N; ++i) 
			hashes[i] = events[i].nameHash;

		ResourcePreloadEvents e{ResourceType::SHADER_STAGE, N, hashes};
		frm->OnEventsByType(&e, 1, ResourceEventType::PRELOAD);*/

		return N;
	}

	u32 LoadPrograms(Engine* engine) {
		CResourceModule* frm = engine->resourceModule;
		const u32 N = 1;

		const char* stages[] = {"Shaders/simpleLineVertex.glsl", "Shaders/simpleLineFragment.glsl"};
		ShaderProgramDescription program;
		program.vertexStage = core::ToHash(stages[0], (u32) strlen(stages[0]));
		program.fragmentStage = core::ToHash(stages[1], (u32) strlen(stages[1]));
		program.hash = core::ToHash("square", 7);

		u64 type = static_cast<u64>(ResourceType::SHADER_PROGRAM);
		char* data = reinterpret_cast<char*>(&program);

		u64 sizes = 0;

		ResourceRegisterEvents evt;
		evt.typeIDs = &type;
		evt.buffers = &data;
		evt.hashes = &program.hash;
		evt.bufferSizes = &sizes;
		evt.count = 1;
		frm->OnEventsByType(&evt, ResourceEventType::REGISTER);

		return 0;
	}

	u32 LoadTextures(Engine* engine) {
		CFileSystemModule* fsm = engine->fileSystemModule;
		CResourceModule* frm = engine->resourceModule;
		const u32 N = 1;

		const char* paths[] = {"wall.png"};
		h64 hashes[N];
		for (u32 i = 0; i < N; ++i) {
			hashes[i] = core::ToHash(paths[i], (u32) strlen(paths[i]));
		}

		FileSystemRegisterPathEvents evt;
		evt.paths = paths;
		evt.hashes = hashes;

		fsm->OnEventsByType(&evt, FileSystemEventType::REGISTER_PATH);

	/*	u64 type = static_cast<u64>(ResourceType::TEXTURE);

		ResourcePreloadEvents e{N, &type, hashes};
		frm->OnEventsByType(&e,ResourceEventType::PRELOAD);*/

		return 1;
	}

	u32 LoadMaterialTemplates(Engine* engine) {
		CResourceModule* frm = engine->resourceModule;
		const u32 N = 1;

		CMaterialTemplate materialTemplate;

		h64 texture[N] = {core::ToHash("diffuseTexture", 15)};
		 
		const char* name[N] = {"material1"};
		materialTemplate.Init(core::ToHash(name[0], 10), 0);
		materialTemplate.SetTextures(texture, N);
		// materialTemplate.SetUniforms();

		/*u64 type = static_cast<u64>(ResourceType::MATERIAL_TEMPLATE);
		char* data = reinterpret_cast<char*>(&materialTemplate);
		u64 sizes = 0;
		h64 h = materialTemplate.Name();

		ResourceRegisterEvents evt;
		evt.typesID = &type;
		evt.buffers = &data;
		evt.hashes = &h;
		evt.buffersSize = &sizes;
		evt.count = 1;
		frm->OnEventsByType(&evt, ResourceEventType::REGISTER);*/

		return 0;
	}

	u32 LoadMaterials(Engine* engine) {
		CResourceModule* frm = engine->resourceModule;
		const u32 N = 1;

		h64 textures[N] = {core::ToHash("wall.png", 9)};

		MaterialDescription material;
		material.shaderProgram = 0;
		material.templateHash = core::ToHash("material1", 10);
		material.texturesCount = N;
		material.textures = textures; // ---> this cant be stored
		material.hash = core::ToHash("material1_wall", 15);

		u64 type = static_cast<u64>(ResourceType::MATERIAL);
		char* data = reinterpret_cast<char*>(&material);

		h64 h = core::ToHash("material1_wall", 15);
		u64 sizes = 0;
		ResourceRegisterEvents evt;
		evt.typeIDs = &type;
		evt.buffers = &data;
		evt.bufferSizes = &sizes;
		evt.hashes = &h;
		evt.count = 1;

		//frm->OnEventsByType(&evt, N, ResourceEventType::REGISTER);
		return 0;
	}

	u32 LoadMeshes(Engine* engine) {
		CResourceModule* frm = engine->resourceModule;
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
		mesh.indexBuffer.indexType = IndexBufferType::U32;
		mesh.hash = core::ToHash("mesh1", 6);


		u64 type = static_cast<u64>(ResourceType::MESH);
		char* data = reinterpret_cast<char*>(&mesh);
		u64 sizes = 0;
		h64 h = core::ToHash("mesh1", 6);

		ResourceRegisterEvents evt;
		evt.typeIDs = &type;
		evt.buffers = &data;
		evt.bufferSizes = &sizes;
		evt.hashes = &h;
		evt.count = 1;
		//frm->OnEventsByType(&evt, N, ResourceEventType::REGISTER);
		return 0;
	}


	u32 LoadRenderObjects(Engine* engine) {
		CResourceModule* frm = engine->resourceModule;
		const u32 N = 1;

		RenderObjectDescription ro;
		ro.hash = core::ToHash("renderObject1", 14);
		ro.material = core::ToHash("material1_wall", 15);
		ro.shaderProgram = core::ToHash("square", 7);


		u64 type = static_cast<u64>(ResourceType::RENDER_OBJECT);
		char* data = reinterpret_cast<char*>(&ro);

		h64 h = core::ToHash("renderObject1", 14);
		u64 sizes = 0;
		ResourceRegisterEvents evt;
		evt.typeIDs = &type;
		evt.buffers = &data;
		evt.bufferSizes = &sizes;
		evt.hashes = &h;
		evt.count = 1;

	//	frm->OnEventsByType(&evt, ResourceEventType::REGISTER);
		return 0;
	}


	

}