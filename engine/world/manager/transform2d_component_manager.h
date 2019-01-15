#pragma once

#include <core/collection/hashmap.h>

#include "../entity.h"

struct M44;
struct V2;

namespace core {
	struct IAllocator;
}

namespace eng {
	struct Transform2DComponent {
		core::Handle h;
	};

	class Transform2DComponentManager {
	public:
		Transform2DComponentManager();
		~Transform2DComponentManager();

		void Init(core::IAllocator* allocator);
		void Reserve(u32 capacity);

		Transform2DComponent Find(Entity entity) const;

		Transform2DComponent Create(Entity entity);
		bool Remove(Entity entity);

		f32 Angle(Transform2DComponent handle) const;
		void SetAngle(Transform2DComponent handle, f32 angle);

		const V2& Position(Transform2DComponent handle) const;
		void SetPosition(Transform2DComponent handle, const V2& position);

		const V2& Scale(Transform2DComponent handle) const;
		void SetScale(Transform2DComponent handle, const V2& scale);

		const M44& Transform(Transform2DComponent handle) const;

	private:
		void Reallocate(u32 capacity);
		void SetEmpty(Transform2DComponent handle);

	private:
		struct Data {
			u32 count;
			u32 capacity;
			void* data;

			Entity*   id;
			bool*     dirty;
			f32*      angle;
			V2* position;
			V2* scale;
			M44* transform;
		};

		Data _data;
		core::HashMap<Transform2DComponent> _map;
		core::IAllocator*  _allocator;
	};
}

