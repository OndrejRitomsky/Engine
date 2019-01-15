#include "material_template.h"

#include <core/common/debug.h>
#include <core/algorithm/cstring.h>

namespace eng {
	MaterialTemplate::MaterialTemplate() :
		_texturesCount(0),
		_uniformsCount(0),
		_shaderProgram(0),
	//	_nameHash(0),
		_initialized(true) {

	}

	MaterialTemplate::~MaterialTemplate() {

	}

	void MaterialTemplate::Init(h64 nameHash, h64 shaderProgram) {
		_name= nameHash;
		_shaderProgram = shaderProgram;
		_initialized = true;
	}

	void MaterialTemplate::SetTextures(const h64* textures, u32 count) {
		ASSERT(_initialized);
		ASSERT(count <= MAX_TEXTURES);

		_texturesCount = count;
		for (u32 i = 0; i < count; ++i) {
			_textures[i] = textures[i];
		}
	}

	void MaterialTemplate::SetUniforms(const h64* uniforms, const MaterialUniformType* uniformTypes, u32 count) {
		ASSERT(_initialized);
		ASSERT(count <= MAX_UNIFORMS);

		_uniformsCount = count;
		if (count > 0) {
			core::Memcpy(_uniforms, uniforms, sizeof(core::Handle) * count);
			core::Memcpy(_uniformsTypes, uniformTypes, sizeof(MaterialUniformType) * count);
		}
	}

	u32 MaterialTemplate::Textures(const h64** outTextures) const {
		ASSERT(_initialized);
		*outTextures = _textures;
		return _texturesCount;
	}

	u32 MaterialTemplate::Uniforms(const h64** outUniforms, const MaterialUniformType** outUniformTypes) const {
		ASSERT(_initialized);
		*outUniforms = _uniforms;
		*outUniformTypes = _uniformsTypes;
		return _uniformsCount;
	}

	const h64 MaterialTemplate::ShaderProgram() const {
		ASSERT(_initialized);
		return _shaderProgram;
	}

	const core::Handle MaterialTemplate::Name() const {
		ASSERT(_initialized);
		return _name;
	}
}