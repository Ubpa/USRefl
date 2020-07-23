#pragma once

#include <utility>

namespace Ubpa::USRefl::detail {
	template<typename T, template<typename...> class U>
	struct IsInstance : std::false_type {};
	template<template<typename...>class U, typename... Ts>
	struct IsInstance<U<Ts...>, U> : std::true_type {};

	template<typename Ints> struct IntegerSequenceTraits;

	template<typename T, T N0, T... Ns>
	struct IntegerSequenceTraits<std::integer_sequence<T, N0, Ns...>> {
		static constexpr size_t head = N0;
		static constexpr auto tail = std::integer_sequence<T, Ns...>{};
	};
}
