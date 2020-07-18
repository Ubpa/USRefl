#pragma once

#include <utility>

namespace Ubpa::USRefl::detail {
	template<typename T, template<typename...> class U>
	struct IsInstance : std::false_type {};
	template<template<typename...>class U, typename... Ts>
	struct IsInstance<U<Ts...>, U> : std::true_type {};

	template<typename Indices> struct IndexSequenceTraits;

	template<size_t N0, size_t... Ns>
	struct IndexSequenceTraits<std::index_sequence<N0, Ns...>> {
		static constexpr size_t head = N0;
		static constexpr auto tail = std::index_sequence<Ns...>{};
	};
}
