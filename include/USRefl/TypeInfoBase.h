#pragma once

#include "BaseList.h"
#include "_deps/nameof.hpp"

namespace Ubpa::USRefl {
	template<typename T, typename... Bases>
	struct TypeInfoBase {
		using type = T;
		static constexpr std::string_view name = nameof::nameof_type<T>();
		static constexpr BaseList bases = { Bases{}... };

		template<typename Derived>
		static constexpr auto&& Forward(Derived&& derived) noexcept;

		static constexpr auto VirtualBases();

		template<typename Func, size_t Depth = 0>
		static constexpr void DFS(Func&& func);

		template<typename U, typename Func>
		static constexpr void ForEachVarOf(U&& obj, Func&& func);
	};
}

#include "detail/TypeInfoBase.inl"
