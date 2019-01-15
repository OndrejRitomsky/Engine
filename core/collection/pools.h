#pragma once

#include "../common/types.h"

namespace core {
	struct IAllocator;

	class BufferPool {
		bool Init(IAllocator* allocator, u32 bufferSize, u32 initialCapacity);
		void Deinit();

		char* New();
		void Release(char* element);

	private:
		u32 _bufferSize;
		u32 _used;
		void* _firstFree;
		void* _lastFree;
		IAllocator* _allocator;
	};


	template<typename Type>
	class ObjectPool  {
	public:
		void Init(IAllocator* allocator, u32 initialCapacity);

		void Deinit();

		Type* New();

		void Release(Type* element);

	private:
		BufferPool _pool;
	};


	template<typename Type>
	inline void ObjectPool<Type>::Init(IAllocator* allocator, u32 initialCapacity) {
		_pool.Init(pool, allocator, sizeof(Type), initialCapacity);
	}

	template<typename Type>
	inline void ObjectPool<Type>::Deinit() {
		_pool.Deinit(pool);
	}

	template<typename Type>
	inline Type* ObjectPool<Type>::New() {
		return (Type*) _pool.New(pool);
	}

	template<typename Type>
	inline void ObjectPool<Type>::Release(Type* element) {
		_pool.Release(pool, (char*) element);
	}
}
