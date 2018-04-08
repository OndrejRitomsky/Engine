#include "CRenderPipeline.h"

#include <Core/Common/Assert.h>
#include <Core/Collection/HashMap.inl>

#include <Renderer/API/Resources/Texture.h>
#include <Renderer/API/Resources/Shader.h>
#include <Renderer/API/Resources/Buffer.h>

//#include "Engine/RenderPipeline/Manager/CRenderComponentManager.h"


#include "Engine/Resources/ResourceManager/CMaterialManager.h"
#include "Engine/Resources/ResourceManager/CShaderProgramManager.h"


#include "Engine/Resources/CResourceModule.h"

#include "Engine/Resources/Resource/Resource.h"
#include "Engine/Resources/Resource/Texture.h"
#include "Engine/Resources/Resource/Material.h"
#include "Engine/Resources/Resource/Mesh.h"
#include "Engine/Resources/Resource/ShaderProgram.h"

#include "Engine/Modules/CStaticConstructor.h"

#include "Engine/Util/CHandleManager.h"

namespace eng {
	//---------------------------------------------------------------------------
	CRenderPipeline::CRenderPipeline() :
		_taggedAllocator(nullptr) {
	}

	//---------------------------------------------------------------------------
	CRenderPipeline::~CRenderPipeline() {
		_handleManager->~CHandleManager();
	}

	//---------------------------------------------------------------------------
	void CRenderPipeline::ConstructSubsytems(CStaticConstructor* constructor) {
		_handleManager = constructor->Construct<CHandleManager>();
	}

	//---------------------------------------------------------------------------
	void CRenderPipeline::Init(core::IAllocator* allocator, core::ITagAllocator* taggedAllocator,
				CResourceModule* resourceModule, CRenderComponentManager* renderObjectManager) {

		_taggedAllocator = taggedAllocator;
		_allocator = allocator;
		_resourceModule = resourceModule;
	//	_renderComponentManager = renderObjectManager;

		_handleManager->Init(allocator);
		_textures.Init(allocator);
		_shaders.Init(allocator);
	}


	//---------------------------------------------------------------------------
	void CRenderPipeline::Render(Entity* entity, u32 count) {
		const CMaterialManager* materialManager = _resourceModule->MaterialManager();
		const CShaderProgramManager* shaderProgramManager = _resourceModule->ShaderProgramManager();
		
		for (u32 i = 0; i < count; ++i) {
	/*		RenderComponent ro = _renderComponentManager->Find(entity[i]);

			Resource mat = _renderComponentManager->Material(ro);
			Resource sp = _renderComponentManager->ShaderProgram(ro);

			// OR MAYBE IT has to exists if render object exists (@TODO dependency manager)
			const Material& material = materialManager->GetMaterial(mat);
			const ShaderProgram& shaderProgram = shaderProgramManager->GetShaderProgram(sp);
	*/
			
		}
	}

	//---------------------------------------------------------------------------
	void CRenderPipeline::OnNewTextureResource(h64 hash) {
		// Not new
		if (_textures.Find(hash))
			return;

		// This will have to increase count
	/*	const Texture* resourceTexture; // =  _resourceModule->GetTexture(hash);

		render::Texture texture;
		render::InitResource(&texture, _handleManager->NewHandle(), render::ResourceType::TEXTURE);
		texture.minFilter = render::TextureFilterType::Linear;
		texture.maxFilter = render::TextureFilterType::Linear;
		texture.wrapS = render::TextureWrapType::Repeat;
		texture.wrapT = render::TextureWrapType::Repeat;
		texture.format = render::TextureFormat::RGBA;
		texture.mipmaps = false;
		texture.data = resourceTexture->data;

		_textures.Add(hash, core::move(texture));*/
	}
}



/*

render::Texture LoadWall(u32 handle) {
render::Texture t1;
render::InitResource(&t1, handle, render::ResourceType::TEXTURE);
t1.minFilter = render::TextureFilterType::Linear;
t1.maxFilter = render::TextureFilterType::Linear;
t1.wrapS = render::TextureWrapType::Repeat;
t1.wrapT = render::TextureWrapType::Repeat;
t1.format = render::TextureFormat::RGBA;
t1.mipmaps = false;
t1.data = TextureManager::CreateTexture("wall.png", t1.width, t1.height);
return t1;
}

render::Shader LoadShader(ShaderProgramManagerNew* shaderProgramManagerNew, u32 handle) {
render::Shader s1;
render::InitResource(&s1, handle, render::ResourceType::SHADER);
{
char vName[] = "Shaders/vertex.glsl";
char fName[] = "Shaders/fragment.glsl";

auto* a = shaderProgramManagerNew->GetShaderStage(core::ToHash(vName, (u32) strlen(vName)));
auto* b = shaderProgramManagerNew->GetShaderStage(core::ToHash(fName, (u32) strlen(fName)));
s1.vertexStage.data = a->data;
s1.vertexStage.dataSize = a->dataSize;
s1.fragmentStage.data = b->data;
s1.fragmentStage.dataSize = b->dataSize;
}

return s1;
}

render::IndexBuffer i1;
render::InitResource(&i1, handleManager.NewHandle(), render::ResourceType::INDEX_BUFFER);
i1.data = (char*) indices;
i1.dataSize = sizeof(indices);
i1.bufferType = render::BufferType::STATIC;
i1.indexType = render::IndexType::U32;

render::VertexBuffer v1;
render::InitResource(&v1, handleManager.NewHandle(), render::ResourceType::VERTEX_BUFFER);
v1.data = (char*) vertices;
v1.dataSize = sizeof(vertices);
v1.bufferType = render::BufferType::STATIC;

render::VertexDescription vi1;
render::InitResource(&vi1, handleManager.NewHandle(), render::ResourceType::VERTEX_DESCRIPTION);
vi1.attributesCount = 0;
vi1.indexBufferHandle = i1.handle;
AddVertexAttribute(&vi1, v1.handle, 0, 0, 16, 0, render::VertexAttributeType::V2);
AddVertexAttribute(&vi1, v1.handle, 1, 8, 16, 0, render::VertexAttributeType::V2);


core::SafeTaggedBlockAllocator taggedAllocator;
taggedAllocator.Init(_allocator, 1024 * 1024, 8);
core::SingleTagProxyAllocator renderAllocator(&taggedAllocator, 14);

render::RenderContextStream stream;
stream.Init(&renderAllocator);

render::RenderResourceContext resContext[5];
resContext[0].Allocate(&stream, &s1);
resContext[1].Allocate(&stream, &t1);
resContext[2].Allocate(&stream, &v1);
resContext[3].Allocate(&stream, &i1);
resContext[4].Allocate(&stream, &vi1);



render::BatchDescription batch;
batch.instances = 1;
batch.vertices = 0; // doesnt matter
batch.indices = sizeof(indices) / sizeof(float);
batch.type = render::BatchType::TRIANGLES;
batch.indexType = render::IndexType::U32; // -> actualy device could hold this

render::JobPackage jobPackage;
jobPackage.batch = batch;
jobPackage.shader = s1;
jobPackage.size = sizeof(render::JobPackage) + 1 * sizeof(render::Resource) + 1 * sizeof(render::Sampler2DUniform);
jobPackage.resourcesOffset = sizeof(render::JobPackage);
jobPackage.resourcesCount = 1;
jobPackage.uniformsOffset = jobPackage.resourcesOffset + 1 * sizeof(render::Resource);
jobPackage.uniformsCount = 1;
jobPackage.sortKey = 1;

// @TODO figure out render package, how will the data for rendering be stored, before we send it to render context

render::RenderContext renderContexts[2];
renderContexts[0].Init(&stream);
renderContexts[0].BeginStateCommands(0);
renderContexts[0].AddClearCommand(render::ClearFlags::COLOR);
renderContexts[0].EndStateCommands();

renderContexts[1].Init(&stream);
render::JobPackage* rjp = renderContexts[1].AddRenderJobPackage(&jobPackage);
render::Resource* resource = (render::Resource*) core::mem::Add(rjp, rjp->resourcesOffset);
resource[0] = *((render::Resource*) &vi1);

char* uniforms = (char*) core::mem::Add(rjp, rjp->uniformsOffset);

render::Sampler2DUniform un;
un.handle = t1.handle;
un.nameHash = core::ToHash("diffuseTexture", 15);
Memcpy(uniforms, &un, sizeof(render::Sampler2DUniform));

*/