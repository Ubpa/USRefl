#pragma once

#include "Util.h"

namespace Ubpa::USRefl::detail {
	template<typename List, typename Func, typename Acc, size_t... Ns, bool... masks>
	constexpr auto Accumulate(List list, Func&& func, Acc&& acc, std::index_sequence<Ns...>, std::integer_sequence<bool, masks...>) {
		if constexpr (sizeof...(Ns) > 0) {
			using IST_N = IntegerSequenceTraits<std::index_sequence<Ns...>>;
			if constexpr (sizeof...(masks) > 0) {
				using IST_Mask = IntegerSequenceTraits<std::integer_sequence<bool, masks...>>;
				if constexpr (IST_Mask::head == true) {
					return Accumulate(
						list,
						std::forward<Func>(func),
						func(std::forward<Acc>(acc), list.template Get<IST_N::head>()),
						IST_N::tail,
						IST_Mask::tail
					);
				}
				else { // mask is false
					return Accumulate(
						list,
						std::forward<Func>(func),
						std::forward<Acc>(acc), // directly forward acc
						IST_N::tail,
						IST_Mask::tail
					);
				}
			}
			else { // default true
				return Accumulate(
					list,
					std::forward<Func>(func),
					func(std::forward<Acc>(acc), list.template Get<IST_N::head>()),
					IST_N::tail,
					std::integer_sequence<bool>{}
				);
			}
		}
		else
			return acc;
	}

	template<typename List, typename Func, size_t... Ns>
	constexpr size_t FindIf(List list, Func&& func, std::index_sequence<Ns...>) {
		if constexpr (sizeof...(Ns) > 0) {
			using IST = IntegerSequenceTraits<std::index_sequence<Ns...>>;
			return func(list.template Get<IST::head>()) ?
				IST::head : FindIf(list, std::forward<Func>(func), IST::tail);
		}
		else
			return static_cast<size_t>(-1);
	}
}

namespace Ubpa::USRefl {
	template<typename... Elems>
	template<bool... masks, typename Init, typename Func>
	constexpr auto ElemList<Elems...>::Accumulate(Init&& init, Func&& func) const {
		return detail::Accumulate(
			*this,
			std::forward<Func>(func),
			std::forward<Init>(init),
			std::make_index_sequence<size>{},
			std::integer_sequence<bool, masks...>{}
		);
	}

	template<typename... Elems>
	template<bool... masks, typename Func>
	constexpr void ElemList<Elems...>::ForEach(Func&& func) const {
		// std::apply([&](auto... elems) { (std::forward<Func>(func)(elems), ...); }, elems);
		Accumulate<masks...>(0, [&](auto, auto field) {
			std::forward<Func>(func)(field);
			return 0;
		});
	}

	template<typename... Elems>
	template<typename Func>
	constexpr size_t ElemList<Elems...>::FindIf(Func&& func) const {
		return detail::FindIf(*this, std::forward<Func>(func), std::make_index_sequence<size>{});
	}

	template<typename... Elems>
	constexpr size_t ElemList<Elems...>::Find(std::string_view name) const {
		return FindIf([name](auto elem) { return elem.name == name; });
	}

	template<typename... Elems>
	template<typename T>
	constexpr size_t ElemList<Elems...>::FindValue(T value) const {
		return FindIf([value](auto e) { return e.value == value; });
	}

	template<typename... Elems>
	constexpr bool ElemList<Elems...>::Contains(std::string_view name) const {
		return Find(name) != static_cast<size_t>(-1);
	}

	template<typename... Elems>
	template<size_t N>
	constexpr auto ElemList<Elems...>::Get() const {
		static_assert(N != static_cast<size_t>(-1));
		return std::get<N>(elems);
	}

	template<typename... Elems>
	template<typename Elem>
	constexpr auto ElemList<Elems...>::Push(Elem e) const {
		return std::apply([e](auto... elems) {
			return ElemList<Elems..., Elem>{ elems..., e };
		}, elems);
	}

	template<typename... Elems>
	template<typename Elem>
	constexpr auto ElemList<Elems...>::Insert(Elem e) const {
		if constexpr ((std::is_same_v<Elems, Elem> || ...))
			return *this;
		else
			return Push(e);
	}
}
