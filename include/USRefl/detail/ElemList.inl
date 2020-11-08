#pragma once

#include "../Util.h"

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
	template<typename Name>
	constexpr auto ElemList<Elems...>::Find(Name) const {
		return Accumulate(nullptr, [](auto acc, auto ele) {
			using Elem = std::decay_t<decltype(ele)>;
			if constexpr (!std::is_same_v<std::decay_t<decltype(acc)>, nullptr_t>)
				return acc;
			else if constexpr (Elem::template NameIs<Name>())
				return ele;
			else
				return acc;
		});
	}

	template<typename... Elems>
	template<typename T>
	constexpr size_t ElemList<Elems...>::FindValue(T value) const {
		return FindIf([value](auto e) { return e.value == value; });
	}

	template<typename... Elems>
	template<typename T, typename Char>
	constexpr T ElemList<Elems...>::ValueOfName(std::basic_string_view<Char> name) const {
		T value{};
		FindIf([name, &value](auto ele) {
			using Elem = std::decay_t<decltype(ele)>;
			if constexpr (std::is_same_v<typename Elem::Tag::Char, Char> && Elem::template ValueTypeIs<T>()) {
				if (ele.name == name) {
					value = ele.value;
					return true;
				}
				else
					return false;
			}
			else
				return false;
		});
		return value;
	}

	template<typename... Elems>
	template<typename T, typename Char>
	constexpr std::basic_string_view<Char> ElemList<Elems...>::NameOfValue(T value) const {
		std::basic_string_view<Char> name;
		FindIf([value, &name](auto ele) {
			using Elem = std::decay_t<decltype(ele)>;
			if constexpr (Elem::template ValueTypeIs<T>()) {
				if (ele.value == value) {
					name = Elem::name;
					return true;
				}
				else
					return false;
			}
			else
				return false;
		});
		return name;
	}

	template<typename... Elems>
	template<typename Name>
	constexpr bool ElemList<Elems...>::Contains(Name) {
		return (Elems::template NameIs<Name>() || ...);
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
