#pragma once

namespace Ubpa::USRefl::detail {
	template<typename TI, typename U, typename Func>
	constexpr void ForEachNonVirtualVarOf(TI info, U&& obj, Func&& func) {
		info.fields.ForEach([&](auto field) {
			if constexpr (!field.is_static && !field.is_func)
				std::forward<Func>(func)(std::forward<U>(obj).*(field.value));
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
		static_assert(std::is_base_of_v<type, std::decay_t<Derived>>);
		using DecayDerived = std::decay_t<Derived>;
		if constexpr (std::is_same_v<const DecayDerived&, Derived>)
			return static_cast<const type&>(derived);
		else if constexpr (std::is_same_v<DecayDerived&, Derived>)
			return static_cast<type&>(derived);
		else if constexpr (std::is_same_v<DecayDerived, Derived>)
			return static_cast<type&&>(derived);
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

	template<typename T, typename... Bases>
	template<typename Func, size_t Depth>
	constexpr void TypeInfoBase<T, Bases...>::DFS(Func&& func) {
		func(TypeInfo<type>{}, Depth);
		if constexpr (Depth == 0) {
			VirtualBases().ForEach([&](auto vb) {
				func(vb, 1);
			});
		}
		bases.ForEach([&](auto base) {
			if constexpr (!base.is_virtual)
				base.info.template DFS<Func, Depth + 1>(std::forward<Func>(func));
		});
	}

	template<typename T, typename... Bases>
	template<typename U, typename Func>
	constexpr void TypeInfoBase<T, Bases...>::ForEachVarOf(U&& obj, Func&& func) {
		static_assert(std::is_same_v<type, std::decay_t<U>>);
		VirtualBases().ForEach([&](auto vb) {
			vb.fields.ForEach([&](auto field) {
				if constexpr (!field.is_static && !field.is_func)
					std::forward<Func>(func)(std::forward<U>(obj).*(field.value));
			});
		});
		detail::ForEachNonVirtualVarOf(TypeInfo<type>{}, std::forward<U>(obj), std::forward<Func>(func));
	}
}
