#include "function.h"

namespace core {
	template<typename RType, typename... Args>
	inline Function<RType(Args...)>::Function() :
		_instance(nullptr),
		_function(nullptr) {
	}

	template<typename RType, typename... Args>
	inline bool Function<RType(Args...)>::IsBound() {
		return _function;
	}

	template<typename RType, typename... Args>
	template<RType(*func)(Args...)>
	inline static Function<RType(Args...)> Function<RType(Args...)>::Make() {
		Function<RType(Args...)> cb;
		cb.Bind<func>();
		return cb;
	}

	template<typename RType, typename... Args>
	template<typename Type, RType(Type::*meth)(Args...)>
	inline static Function<RType(Args...)> Function<RType(Args...)>::Make(void* instance) {
		Function<RType(Args...)> cb;
		cb.Bind<Type, meth>(instance);
		return cb;
	}

	template<typename RType, typename... Args>
	template<typename Type, RType(Type::*meth)(Args...) const>
	inline static Function<RType(Args...)> Function<RType(Args...)>::Make(const void* instance) {
		Function<RType(Args...)> cb;
		cb.Bind<Type, meth>(instance);
		return cb;
	}

	template<typename RType, typename... Args>
	template<RType(*func)(Args...)>
	inline static RType Function<RType(Args...)>::FunctionWrapper(void* instance, Args... args) {
		return func(forward<Args>(args)...);
	}

	template<typename RType, typename... Args>
	template<typename Type, RType(Type::*meth)(Args...)>
	inline static RType Function<RType(Args...)>::MethodWrapper(void* instance, Args... args) {
		ASSERT(instance);
		return (*static_cast<Type*>(instance).*meth)(forward<Args>(args)...);
	}

	template<typename RType, typename... Args>
	template<typename Type, RType(Type::*meth)(Args...) const>
	inline static RType Function<RType(Args...)>::ConstMethodWrapper(void* instance, Args... args) {
		ASSERT(instance);
		return (*const_cast<const Type*>(static_cast<const Type*>(instance)).*meth)(forward<Args>(args)...);
	}

	template<typename RType, typename... Args>
	template<RType(*func)(Args...)>
	inline void Function<RType(Args...)>::Bind() {
		_instance = nullptr;
		_function = FunctionWrapper<func>;
	}

	template<typename RType, typename... Args>
	template<typename Type, RType(Type::*meth)(Args...)>
	inline void Function<RType(Args...)>::Bind(void* instance) {
		_instance = instance;
		_function = MethodWrapper<Type, meth>;
	}

	template<typename RType, typename... Args>
	template<typename Type, RType(Type::*meth)(Args...) const>
	inline void Function<RType(Args...)>::Bind(const void* instance) {
		_instance = const_cast<void*>(instance);
		_function = ConstMethodWrapper<Type, meth>;
	}

	template<typename RType, typename... Args>
	template<typename... BArgs>
	inline RType Function<RType(Args...)>::operator()(BArgs&&... args) {
		return (*_function)(_instance, forward<BArgs>(args)...);
	}

	template<typename RType, typename... Args>
	template<typename... BArgs>
	inline RType Function<RType(Args...)>::operator()(BArgs&&... args) const {
		return (*_function)(_instance, forward<BArgs>(args)...);
	}

	template<typename RType, typename... Args>
	inline bool Function<RType(Args...)>::operator==(Function<RType(Args...)> oth) const {
		return _instance == oth._instance && _function == oth._function;
	}

}