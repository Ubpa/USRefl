#pragma once

#include <utility>
#include <cassert>

namespace Ubpa::USRefl::detail {
	template<typename T, template<typename...> class U>
	struct IsInstance : std::false_type {};
	template<template<typename...>class U, typename... Ts>
	struct IsInstance<U<Ts...>, U> : std::true_type {};

	template<typename Ints> struct IntegerSequenceTraits;

	template<typename T, T N0, T... Ns>
	struct IntegerSequenceTraits<std::integer_sequence<T, N0, Ns...>> {
		static constexpr std::size_t head = N0;
		static constexpr auto tail = std::integer_sequence<T, Ns...>{};
	};

	template<typename Signature>
	struct ConstructorWrapper;
	template<typename T, typename... Args>
	struct ConstructorWrapper<T(Args...)> {
		static constexpr auto run() {
			return static_cast<void(*)(T*, Args...)>(
				[](T* ptr, Args... args) {
#ifdef assert
					assert(ptr != nullptr);
#endif
					/*return*/ new(ptr) T{ std::forward<Args>(args)... };
				}
			);
		}
	};
}

namespace Ubpa::USRefl {
	// Signature : T(Args...)
	// ->
	// void(*)(T*, Args...)
	template<typename Signature>
	constexpr auto WrapConstructor() {
		return detail::ConstructorWrapper<Signature>::run();
	}

	template<typename T>
	constexpr auto WrapDestructor() {
		return static_cast<void(*)(T*)>(
			[](T* ptr) {
				assert(ptr != nullptr);
				ptr->~T();
			}
		);
	}
}
