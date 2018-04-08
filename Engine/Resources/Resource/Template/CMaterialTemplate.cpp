#include "CMaterialTemplate.h"

#include <Core/Common/Assert.h>
#include <Core/Algorithm/Memory.h>

namespace eng {
	//---------------------------------------------------------------------------
	CMaterialTemplate::CMaterialTemplate() :
		_texturesCount(0),
		_uniformsCount(0),
		_shaderProgram(0),
	//	_nameHash(0),
		_initialized(true) {

	}

	//---------------------------------------------------------------------------
	CMaterialTemplate::~CMaterialTemplate() {

	}

	//---------------------------------------------------------------------------
	void CMaterialTemplate::Init(h64 nameHash, core::Handle shaderProgram) {
		_name= nameHash;
		_shaderProgram = shaderProgram;
		_initialized = true;
	}

	//---------------------------------------------------------------------------
	void CMaterialTemplate::SetTextures(const h64* textures, u32 count) {
		ASSERT(_initialized);
		ASSERT(count <= MAX_TEXTURES);

		_texturesCount = count;
		for (u32 i = 0; i < count; ++i) {
			_textures[i] = textures[i];
		}
	}

	//---------------------------------------------------------------------------
	void CMaterialTemplate::SetUniforms(const h64* uniforms, const MaterialUniformType* uniformTypes, u32 count) {
		ASSERT(_initialized);
		ASSERT(count <= MAX_UNIFORMS);

		_uniformsCount = count;
		Memcpy(_uniforms, uniforms, sizeof(core::Handle) * count);
		Memcpy(_uniformsTypes, uniformTypes, sizeof(MaterialUniformType) * count);
	}

	//---------------------------------------------------------------------------
	u32 CMaterialTemplate::Textures(const h64** outTextures) const {
		ASSERT(_initialized);
		*outTextures = _textures;
		return _texturesCount;
	}

	//---------------------------------------------------------------------------
	u32 CMaterialTemplate::Uniforms(const h64** outUniforms, const MaterialUniformType** outUniformTypes) const {
		ASSERT(_initialized);
		*outUniforms = _uniforms;
		*outUniformTypes = _uniformsTypes;
		return _uniformsCount;
	}

	//---------------------------------------------------------------------------
	const h64 CMaterialTemplate::ShaderProgram() const {
		ASSERT(_initialized);
		return _shaderProgram;
	}

	//---------------------------------------------------------------------------
	const core::Handle CMaterialTemplate::Name() const {
		ASSERT(_initialized);
		return _name;
	}

}