#pragma once

#include "BaseList.h"

#include <UTemplate/Name.hpp>

namespace Ubpa::USRefl {
	template<typename T, typename... Bases>
	struct TypeInfoBase {
		using Type = T;
		using TName = decltype(type_name<T>());
		static constexpr std::string_view name = TName::View();
		static constexpr BaseList bases = { Bases{}... };

		template<typename Derived>
		static constexpr auto&& Forward(Derived&& derived) noexcept;

		static constexpr auto VirtualBases();

		template<typename Init, typename Func>
		static constexpr auto DFS_Acc(Init&& init, Func&& func);

		template<typename Func>
		static constexpr void DFS_ForEach(Func&& func);

		template<typename U, typename Func>
		static constexpr void ForEachVarOf(U&& obj, Func&& func);
	};
}

#include "detail/TypeInfoBase.inl"
