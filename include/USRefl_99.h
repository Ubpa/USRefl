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
	constexpr void ForEach(List list, Func&& func, std::index_sequence<Ns...>){(func(list.template Get<Ns>()), ...);}
	template<typename List, typename Func, size_t... Ns>
	constexpr size_t FindIf(const List& list, Func&& func, std::index_sequence<Ns...>) {
		if constexpr (sizeof...(Ns) > 0) {
			using IST = ISTraits<std::index_sequence<Ns...>>;
			return func(list.template Get<IST::head>()) ? IST::head : FindIf(list, std::forward<Func>(func), IST::tail);
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
		template<typename Func> constexpr void ForEach(Func&& func) const
		{ detail::ForEach(*this, std::forward<Func>(func), std::make_index_sequence<size>{}); }
		template<typename Func> constexpr size_t FindIf(Func&& func) const
		{ return detail::FindIf(*this, std::forward<Func>(func), std::make_index_sequence<size>{}); }
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
#define USRefl_BaseList_GetByName(list, name) list.Get<list.Find(name)>()
#define USRefl_BaseList_GetByValue(list, value) list.Get<list.FindByValue(value)>()
	};
}
template<typename T> struct Attr : detail::NamedValue<T>
{ constexpr Attr(std::string_view name, T value) : detail::NamedValue<T>{ name,value } {} };
template<> struct Attr<void> : detail::NamedValue<void>
{ constexpr Attr(std::string_view name) : detail::NamedValue<void>{ name } {} };
template<size_t N> Attr(std::string_view, const char(&)[N])->Attr<std::string_view>;
Attr(std::string_view)->Attr<void>;
template<typename... Attrs> struct AttrList : detail::BaseList<Attrs...>
{ constexpr AttrList(Attrs... attrs) : detail::BaseList<Attrs...>{ attrs... } {} };
template<bool s, bool f> struct FTraitsB { static constexpr bool is_static = s, is_func = f; };
template<typename T> struct FTraits : FTraitsB<true, false> {}; // default is enum
template<typename U, typename T> struct FTraits<T U::*> : FTraitsB<false, detail::IsFunc<T>::value> {};
template<typename T> struct FTraits<T*> : FTraitsB<true, detail::IsFunc<T>::value>{}; // static member
template<typename T, typename AList> struct Field : FTraits<T>, detail::NamedValue<T> {
	AList attrs;
	constexpr Field(std::string_view n, T v, AList as = {}) : detail::NamedValue<T>{ n,v }, attrs{ as } {}
};
template<typename T, typename AList> Field(std::string_view, T, AList)->Field<T, AList>;
template<typename T> Field(std::string_view, T)->Field<T, AttrList<>>;
template<typename... Fields> struct FieldList : detail::BaseList<Fields...>
{ constexpr FieldList(Fields... fields) : detail::BaseList<Fields...>{ fields... } {}; };
template<typename T> struct TypeInfo; // TypeInfoBase, name, fields, attrs
template<typename... TypeInfos> struct TypeInfoList : detail::BaseList<TypeInfos...>
{ constexpr TypeInfoList(TypeInfos... typeInfos) : detail::BaseList<TypeInfos...>{ typeInfos... } {}; };
template<typename T, typename... Bases> struct TypeInfoBase {
	using type = T;
	static constexpr TypeInfoList bases = { TypeInfo<Bases>{}... };
	template<typename U> constexpr auto&& Forward(U&& derived) noexcept {
		if constexpr (std::is_same_v<const std::decay_t<U>&, U>) return static_cast<const type&>(derived);
		else if constexpr (std::is_same_v<std::decay_t<U>&, U>) return static_cast<type&>(derived);
		else return static_cast<type&&>(derived); // if constexpr (std::is_same_v<DecayU, U>)
	}
	template<typename Func> static constexpr void DFS(Func&& func, size_t depth = 0) {
		func(TypeInfo<type>{}, depth);
		TypeInfo<type>::bases.ForEach([&](auto base) { base.DFS(std::forward<Func>(func), depth + 1); });
	}
	template<typename U, typename Func> static constexpr void ForEachVarOf(U&& obj, Func&& func) {
		TypeInfo<type>::fields.ForEach([&](auto f)
		{ if constexpr (!f.is_static && !f.is_func)std::forward<Func>(func)(std::forward<U>(obj).*(f.value)); });
		TypeInfo<type>::bases.ForEach([&](auto base)
		{ base.ForEachVarOf(base.Forward(std::forward<U>(obj)), std::forward<Func>(func)); });
	}
};