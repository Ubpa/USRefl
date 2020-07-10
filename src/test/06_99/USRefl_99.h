#pragma once // Ubpa Static Reflection 99
#include <tuple>
#include <string_view>
namespace detail {
	template<typename Indices> struct ISTraits; // index sequence traits
	template<size_t N0, size_t... Ns> struct ISTraits<std::index_sequence<N0, Ns...>> {
		static constexpr size_t head = N0;
		static constexpr auto tail = std::index_sequence<Ns...>{};
	};
	template<typename F> struct IsFunc : std::false_type {};
	template<typename R, typename... Args> struct IsFunc<R(Args...)> : std::true_type {};
	template<typename R, typename... Args> struct IsFunc<R(Args...)const> : std::true_type {};
	template<typename List, typename Func, size_t... Ns>
	constexpr void ForEach(List list, const Func& func, std::index_sequence<Ns...>)
	{ (func(list.Get<Ns>()), ...); }
	template<typename List, typename Func, size_t... Ns>
	constexpr size_t FindIf(const List& list, const Func& func, std::index_sequence<Ns...>) {
		if constexpr (sizeof...(Ns) > 0) {
			using IST = ISTraits<std::index_sequence<Ns...>>;
			return func(list.Get<IST::head>()) ? IST::head : FindIf(list, func, IST::tail);
		} else return static_cast<size_t>(-1);
	}
	template<typename T> struct NamedValue {
		std::string_view name; T value;
		static constexpr bool has_value = !std::is_void_v<T>;
	};
	template<> struct NamedValue<void> {
		std::string_view name; /*T value;*/
		static constexpr bool has_value = false;
	};
	template<typename...Elems> struct BaseList {
		std::tuple<Elems...> list;
		static constexpr size_t size = sizeof...(Elems);
		constexpr BaseList(Elems... elems) : list{ elems... } {}
		template<typename Func> constexpr void ForEach(const Func& func) const
		{ detail::ForEach(*this, func, std::make_index_sequence<size>{}); }
		template<typename Func> constexpr size_t FindIf(const Func& func) const
		{ return detail::FindIf(*this, func, std::make_index_sequence<size>{}); }
		constexpr size_t Find(std::string_view n) const { return FindIf([n](auto e){return e.name == n;}); }
		template<typename T> constexpr size_t FindByValue(T value) const {
			return FindIf([value](auto e) {
				if constexpr (e.has_value) {
					if constexpr (std::is_same_v<decltype(e.value), T>) return e.value == value;
					else return false;
				} else return false;
			});
		}
		constexpr bool Contains(std::string_view name) const { return Find(name) != static_cast<size_t>(-1); }
		template<size_t N> constexpr auto Get() const { return std::get<N>(list); }
	};
}
template<typename T> struct Attr : detail::NamedValue<T>
{ constexpr Attr(std::string_view name, T value) : detail::NamedValue<T>{ name,value } {} };
template<> struct Attr<void> : detail::NamedValue<void>
{ constexpr Attr(std::string_view name) : detail::NamedValue<void>{ name } {} };
template<size_t N> Attr(std::string_view, const char[N])->Attr<std::string_view>;
Attr(std::string_view)->Attr<void>;
template<typename... Attrs> struct AttrList : detail::BaseList<Attrs...>
{ using detail::BaseList<Attrs...>::BaseList; };
template<typename... Attrs> AttrList(Attrs...)->AttrList<Attrs...>;
template<bool s, bool f> struct FTraitsB {
	static constexpr bool is_static = s, is_func = f;
};
template<typename T> struct FTraits : FTraitsB<true, false> {}; // default is enum
template<typename U, typename T> struct FTraits<T U::*> : FTraitsB<false, detail::IsFunc<T>::value> {};
template<typename T> struct FTraits<T*> : FTraitsB<true, detail::IsFunc<T>::value>{}; // static member
template<typename T, typename AList> struct Field : FTraits<T>, detail::NamedValue<T> {
	AList attrs;
	constexpr Field(std::string_view n, T v, AList as = {}) : detail::NamedValue<T>{ n,v }, attrs{ as } {}
};
template<typename T, typename AList> Field(std::string_view, T, AList)->Field<T, AList>;
template<typename T> Field(std::string_view, T)->Field<T, AttrList<>>;
template<typename... Fields> struct FieldList : detail::BaseList<Fields...> {
	using detail::BaseList<Fields...>::BaseList;
};
template<typename... Fields> FieldList(Fields...)->FieldList<Fields...>;
template<typename T> struct TypeInfo; // name, TypeInfoBase, fields, attrs
template<typename... Ts> struct TypeInfoList : detail::BaseList<Ts...>
{ using detail::BaseList<Ts...>::BaseList; };
template<typename... Ts> TypeInfoList(Ts...)->TypeInfoList<Ts...>;
template<typename T, typename... Bases> struct TypeInfoBase {
	using type = T;
	static constexpr TypeInfoList bases = { TypeInfo<Bases>{}... };
	template<typename U> constexpr auto&& Forward(U&& derived) noexcept {
		if constexpr (std::is_same_v<const std::decay_t<U>&, U>) return static_cast<const type&>(derived);
		else if constexpr (std::is_same_v<std::decay_t<U>&, U>) return static_cast<type&>(derived);
		else return static_cast<type&&>(derived); // if constexpr (std::is_same_v<DecayU, U>)
	}
	template<typename Func> static constexpr void DFS(const Func& func, size_t depth = 0) {
		func(TypeInfo<type>{}, depth);
		TypeInfo<type>::bases.ForEach([&](auto base) { base.DFS(func, depth + 1); });
	}
	template<typename U, typename Func> static constexpr void ForEachVarOf(U&& obj, const Func& func) {
		TypeInfo<type>::fields.ForEach([&](auto f)
		{ if constexpr (!f.is_static && !f.is_func)func(std::forward<U>(obj).*(f.value)); });
		TypeInfo<type>::bases.ForEach([&](auto base)
		{ base.ForEachVarOf(base.Forward(std::forward<U>(obj)), func); });
	}
};