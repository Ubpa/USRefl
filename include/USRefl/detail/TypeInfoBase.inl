#pragma once

namespace Ubpa::USRefl::detail {
	template<typename TI, typename U, typename Func>
	constexpr void ForEachNonVirtualVarOf(TI info, U&& obj, Func&& func) {
		info.fields.ForEach([&](const auto& field) {
			using Fld = std::remove_const_t<std::remove_reference_t<decltype(field)>>;
			if constexpr (!Fld::is_static && !Fld::is_func)
				std::forward<Func>(func)(field, std::forward<U>(obj).*(field.value));
		});
		info.bases.ForEach([&](auto base) {
			if constexpr (!base.is_virtual) {
				ForEachNonVirtualVarOf(
					base.info,
					base.info.Forward(std::forward<U>(obj)),
					std::forward<Func>(func)
				);
			}
		});
	}
}

namespace Ubpa::USRefl {
	template<typename T, typename... Bases>
	template<typename Derived>
	constexpr auto&& TypeInfoBase<T, Bases...>::Forward(Derived&& derived) noexcept {
		static_assert(std::is_base_of_v<Type, std::decay_t<Derived>>);
		using DecayDerived = std::decay_t<Derived>;
		if constexpr (std::is_same_v<const DecayDerived&, Derived>)
			return static_cast<const Type&>(derived);
		else if constexpr (std::is_same_v<DecayDerived&, Derived>)
			return static_cast<Type&>(derived);
		else if constexpr (std::is_same_v<DecayDerived, Derived>)
			return static_cast<Type&&>(derived);
		else
			static_assert(true); // volitile
	}

	template<typename T, typename... Bases>
	constexpr auto TypeInfoBase<T, Bases...>::VirtualBases() {
		return bases.Accumulate(ElemList<>{}, [](auto acc, auto base) {
			constexpr auto vbs = base.info.VirtualBases();
			auto concated = vbs.Accumulate(acc, [](auto acc, auto vb) {
				return acc.Insert(vb);
			});
			if constexpr (base.is_virtual)
				return concated.Insert(base.info);
			else
				return concated;
		});
	}

	template<std::size_t Depth, typename T, typename Acc, typename Func>
	constexpr auto detail_DFS_Acc(T type, Acc&& acc, Func&& func) {
		return type.bases.Accumulate(std::forward<Acc>(acc), [&](auto&& acc, auto base) {
			if constexpr (base.is_virtual) {
				return detail_DFS_Acc<Depth + 1>(
					base.info,
					std::forward<decltype(acc)>(acc),
					std::forward<Func>(func)
				);
			}
			else {
				return detail_DFS_Acc<Depth + 1>(
					base.info,
					std::forward<Func>(func)(std::forward<decltype(acc)>(acc), base.info, Depth + 1),
					std::forward<Func>(func)
				);
			}
		});
	}

	template<typename T, typename... Bases>
	template<typename Init, typename Func>
	constexpr auto TypeInfoBase<T, Bases...>::DFS_Acc(Init&& init, Func&& func) {
		return detail_DFS_Acc<0>(
			TypeInfo<Type>{},
			VirtualBases().Accumulate(std::forward<Func>(func)(std::forward<Init>(init), TypeInfo<Type>{}, 0), [&](auto&& acc, auto vb) {
				return std::forward<Func>(func)(std::forward<decltype(acc)>(acc), vb, 1);
			}),
			std::forward<Func>(func)
		);
	}

	template<typename T, typename... Bases>
	template<typename Func>
	constexpr void TypeInfoBase<T, Bases...>::DFS_ForEach(Func&& func) {
		DFS_Acc(0, [&](auto, auto t, std::size_t depth) {
			std::forward<Func>(func)(t, depth);
			return 0;
		});
	}

	template<typename T, typename... Bases>
	template<typename U, typename Func>
	constexpr void TypeInfoBase<T, Bases...>::ForEachVarOf(U&& obj, Func&& func) {
		static_assert(std::is_same_v<Type, std::decay_t<U>>);
		VirtualBases().ForEach([&](auto vb) {
			vb.fields.ForEach([&](const auto& field) {
				using Fld =  std::remove_const_t<std::remove_reference_t<decltype(field)>>;
				if constexpr (!Fld::is_static && !Fld::is_func)
					std::forward<Func>(func)(field, std::forward<U>(obj).*(field.value));
			});
		});
		detail::ForEachNonVirtualVarOf(TypeInfo<Type>{}, std::forward<U>(obj), std::forward<Func>(func));
	}
}
