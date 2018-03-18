#pragma once


#include "Core/Types.h"
#include "Core/Utility.h"
#include "Core/CoreTypes.h"

#include "Core/Allocator/IAllocator.h"

#include "Core/Collection/Array.h"
#include "Core/Collection/Array.inl"
#include "Core/Collection/Function.h"


namespace core {

	/**
	* HandleMap<Type> : O(1) lookup container
	* Double indirection for lookup
	* Data are contiguous, iterating is without indirection
	*/

	// @TODO @REWORK 3 arrays into SOA
	template<typename Type>
	class HandleMap {
	private:
		static const u32 INTERNAL_INVALID_INDEX = U32MAX;

		union InternalHandle {
			struct {
				u32 index;
				u16 generation;
				u16 type : 15;
				u16 free : 1;
			};

			Handle value;
		};

	public:
		HandleMap(IAllocator* allocator, u16 handleType = 0);
		HandleMap(const HandleMap& oth) = delete;
		~HandleMap();

		HandleMap& operator=(const HandleMap& oth) = delete;

		void Init(IAllocator* allocator);

		// Handle has to be valid!
		inline Type& Get(const Handle& handle);

		// Handle has to be valid!
		inline const Type& Get(const Handle& handle) const;

		inline void Reserve(u32 capacity);

		inline bool IsValid(const Handle& handle) const;

		inline u32 Size() const;
		inline u32 Capacity() const;

		inline Type* begin();
		inline Type* end();

		inline const Type* cbegin() const;
		inline const Type* cend() const;

		Handle Add(const Type& value);

		void Remove(const Handle& handle);

		template<typename... Args>
		Handle FindHandle(Function<bool(const Type&, Args...)> func, Args&&... args) const;

	private:
		u32 _freeHandleFirst;
		u32 _freeHandleLast;

		u16 _handleType;

		Array<InternalHandle> _handles;

		Array<u32>  _valuesToHandles;
		Array<Type> _values;
	};
	
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------

	//---------------------------------------------------------------------------
	template<typename Type>
	HandleMap<Type>::HandleMap(IAllocator* allocator, u16 handleType) :
		_freeHandleFirst(INTERNAL_INVALID_INDEX),
		_freeHandleLast(INTERNAL_INVALID_INDEX),
		_handleType(handleType) {
		_values.Init(allocator);
		_handles.Init(allocator);
		_valuesToHandles.Init(allocator);
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	HandleMap<Type>::~HandleMap() {
	}


	//---------------------------------------------------------------------------
	template<typename Type>
	inline void HandleMap<Type>::Init(IAllocator* allocator) {
		_values.Init(allocator);
		_handles.Init(allocator);
		_valuesToHandles.Init(allocator);
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline Type& HandleMap<Type>::Get(const Handle& handle) {
		ASSERT(IsValid(handle));
		
		InternalHandle iHandle;
		iHandle.value = handle;

		InternalHandle innerHandle = _handles[iHandle.index];
		return _values[innerHandle.index];
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline const Type& HandleMap<Type>::Get(const Handle& handle) const {
		ASSERT(IsValid(handle));

		InternalHandle iHandle;
		iHandle.value = handle;

		Handle innerHandle = _handles[iHandle.index];
		return _values[innerHandle.index];
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline Type* HandleMap<Type>::begin() {
		return _values.begin();
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline Type* HandleMap<Type>::end() {
		return _values.end();
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline const Type* HandleMap<Type>::cbegin() const {
		return _values.cbegin();
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline const Type* HandleMap<Type>::cend() const {
		return _values.cend();
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline bool HandleMap<Type>::IsValid(const Handle& handle) const {
		InternalHandle iHandle;
		iHandle.value = handle;

		if (iHandle.value == INVALID_HANDLE || iHandle.free == 1 || iHandle.type != _handleType || iHandle.index >= _handles.Count())
			return false;

		InternalHandle innerHandle = _handles[iHandle.index];
		return innerHandle.free == 0 && innerHandle.generation == iHandle.generation && innerHandle.index < _values.Count();
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline void HandleMap<Type>::Reserve(u32 capacity) {
		_values.Reserve(capacity);
		_handles.Reserve(capacity);
		_valuesToHandles.Reserve(capacity);
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline u32 HandleMap<Type>::Size() const {
		return _values.Count();
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline u32 HandleMap<Type>::Capacity() const {
		return _values.Capacity();
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	inline typename Handle HandleMap<Type>::Add(const Type& value) {
		InternalHandle handle;
		if (_freeHandleFirst == INTERNAL_INVALID_INDEX) {
			InternalHandle innerHandle;

			innerHandle.index = _values.Count();
			innerHandle.generation = 0;
			innerHandle.type = _handleType;
			innerHandle.free = 0;

			handle = innerHandle;
			handle.index = _handles.Count();

			_handles.Push(innerHandle);

			// if free list is EMPTY, then all array have equal lenghts, therefore we dont need to do reserve
			ASSERT(_values.Count() == _valuesToHandles.Count() && _values.Count() + 1 == _handles.Count());
		}
		else {
			u32 replacedHandleIndex = _freeHandleFirst;

			InternalHandle& innerHandle = _handles[replacedHandleIndex];

			ASSERT(innerHandle.free == 1);

			_freeHandleFirst = innerHandle.index;

			if (_freeHandleFirst == INTERNAL_INVALID_INDEX) {
				_freeHandleLast = _freeHandleFirst;
			}

			innerHandle.index = _values.Count();
			innerHandle.generation++; // overflow is accepted and wanted!
			innerHandle.free = 0;

			handle = innerHandle;
			handle.index = replacedHandleIndex;

			ASSERT(_values.Count() + 1 <= _handles.Count() && _values.Count() == _valuesToHandles.Count());
		}

		_valuesToHandles.Push(handle.index);
		_values.Push(value);

		return handle.value;
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	void HandleMap<Type>::Remove(const Handle& handle) {
		ASSERT(IsValid(handle));

		InternalHandle iHandle;
		iHandle.value = handle;

		InternalHandle& innerHandle = _handles[iHandle.index];

		_values.Remove(innerHandle.index);
		
		_valuesToHandles.Remove(innerHandle.index);

		if (innerHandle.index < _values.Count()) {
			// index wasnt last we need to update handles set
			// new takes now the same place
			u32 fixHandleIndex = _valuesToHandles[innerHandle.index];
			_handles[fixHandleIndex].index = innerHandle.index;
		}

		innerHandle.index = INTERNAL_INVALID_INDEX;
		innerHandle.free = 1;

		if (_freeHandleFirst == INTERNAL_INVALID_INDEX) {
			_freeHandleFirst = iHandle.index;
			_freeHandleLast = _freeHandleFirst;
		}
		else {
			_handles[_freeHandleLast].index = iHandle.index;
			_freeHandleLast = iHandle.index;
		}
	}

	//---------------------------------------------------------------------------
	template<typename Type>
	template<typename... Args>
	typename Handle HandleMap<Type>::FindHandle(Function<bool(const Type&, Args...)> func, Args&&... args) const {
		InternalHandle handle;
		handle.value = INVALID_HANDLE;

		for (u32 i = 0; i < _values.Count(); ++i) {
			if (func(_values[i], forward<Args>(args)...)) {
				u32 index = _valuesToHandles[i];
				handle = _handles[index];
				handle.index = index;
				break;
			}
		}

		return handle.value;
	}



}