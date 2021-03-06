#pragma once

#include <core/common/types.h>

#include "../resources.h" // @TODO hmmm leaking?

namespace eng {
	// @TODO struct?
	// Material template has no description... 
	class MaterialTemplate {
	private:
		static const u32 MAX_TEXTURES = 10u;
		static const u32 MAX_UNIFORMS = 20u;

	public:
		MaterialTemplate();
		~MaterialTemplate();

		void Init(h64 nameHash, h64 shaderProgram); // @TODO ? maybe the material should query the data from shader

		// all at once
		void SetTextures(const h64* textures, u32 count);
		void SetUniforms(const h64* uniforms, const MaterialUniformType* uniformTypes, u32 count); // @TODO should uniform know about its type or its in the shader
 
		u32 Textures(const h64** outTextures) const;
		u32 Uniforms(const h64** outUniforms, const MaterialUniformType** outUniformTypes) const;

		const h64 ShaderProgram() const;
		const h64 Name() const;

	private:
		MaterialUniformType _uniformsTypes[MAX_UNIFORMS];

		bool _initialized;

		u32 _texturesCount;
		u32 _uniformsCount;

		h64 _textures[MAX_TEXTURES]; // for now its contant max and the lookup is linear	
		h64 _uniforms[MAX_UNIFORMS]; // for now its contant max and the lookup is linear

		h64 _shaderProgram;
		h64 _name; // not needeD?
	};

}