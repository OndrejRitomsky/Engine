#pragma once

#include "../common/utility.h"
#include "../common/debug.h"
#include "../algorithm/move.h"

namespace core {

	template<typename>
	class Function;

	template<typename RType, typename... Args>
	class Function<RType(Args...)> {
	private:
		using WrappedFunctionType = RType(*)(void*, Args...);

	public:
		template<RType(*func)(Args...)>
		static Function Make();

		template<typename Type, RType(Type::*meth)(Args...)>
		static Function Make(void* instance);

		template<typename Type, RType(Type::*meth)(Args...) const>
		static Function Make(const void* instance);

	private:
		template<RType(*func)(Args...)>
		static RType FunctionWrapper(void* instance, Args... args);

		template<typename Type, RType(Type::*meth)(Args...)>
		static RType MethodWrapper(void* instance, Args... args);

		template<typename Type, RType(Type::*meth)(Args...) const>
		static RType ConstMethodWrapper(void* instance, Args... args);

	public:
		Function();

		bool IsBound();

		template<RType(*func)(Args...)>
		void Bind();

		template<typename Type, RType(Type::*meth)(Args...)>
		void Bind(void* instance);

		template<typename Type, RType(Type::*meth)(Args...) const>
		void Bind(const void* instance);

		template<typename... BArgs>
		RType operator()(BArgs&&... args);

		template<typename... BArgs>
		RType operator()(BArgs&&... args) const;

		bool operator==(Function<RType(Args...)> oth) const;

	private:
		void* _instance;
		WrappedFunctionType _function;
	};
}