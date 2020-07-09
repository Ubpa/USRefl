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
	{ (func(std::get<Ns>(list)), ...); }
	template<typename List, typename Func, size_t... Ns>
	constexpr size_t FindIf(const List& list, const Func& func, std::index_sequence<Ns...>) {
		if constexpr (sizeof...(Ns) > 0) {
			using IST = ISTraits<std::index_sequence<Ns...>>;
			return func(std::get<IST::head>(list)) ? IST::head : FindIf(list, func, IST::tail);
		} else return static_cast<size_t>(-1);
	}
	template<typename FList, typename TList, size_t... Ns>
	constexpr auto FieldListUnionTypeList(FList flist, TList tlist, std::index_sequence<Ns...>) {
		if constexpr (sizeof...(Ns) > 0) {
			using IST = ISTraits<std::index_sequence<Ns...>>;
			return FieldListUnionTypeList(flist.Union(tlist.Get<IST::head>().fields), tlist, IST::tail);
		} else return flist;
	}
	template<typename T> struct NamedValue {
		std::string_view name; T value;
		static constexpr bool has_value = !std::is_void_v<T>;
	};
	template<> struct NamedValue<void> {
		std::string_view name; /*T value;*/
		static constexpr bool has_value = false;
	};
	template<template<typename...>class ImplT,typename...Elems> struct BaseList {
		std::tuple<Elems...> list;
		static constexpr size_t size = sizeof...(Elems);
		constexpr BaseList(Elems... elems) : list{ elems... } {}
		constexpr BaseList(std::tuple<Elems...> elems) : list{ elems } {}
		template<typename Func> constexpr void ForEach(const Func& func) const
		{ detail::ForEach(list, func, std::make_index_sequence<size>{}); }
		template<typename Func> constexpr size_t FindIf(const Func& func) const
		{ return detail::FindIf(list, func, std::make_index_sequence<size>{}); }
		constexpr size_t Find(std::string_view n) const { return FindIf([n](auto e){return e.name == n;}); }
		template<typename T> constexpr size_t FindByValue(T value) const {
			return FindIf([value](auto elem) {
				if constexpr (elem.has_value) {
					if constexpr (std::is_same_v<decltype(elem.value), decltype(value)>)
						return elem.value == value;
					else return false;
				} else return false;
			});
		}
		constexpr bool Contains(std::string_view name) const { return Find(name) != static_cast<size_t>(-1); }
		template<size_t N> constexpr auto Get() const { return std::get<N>(list); }
		template<typename L> constexpr auto Union(L l) const { return ImplT{ std::tuple_cat(list, l.list) }; }
	};
}
template<typename T> struct Attr : detail::NamedValue<T>
{ constexpr Attr(std::string_view name, T value) : detail::NamedValue<T>{ name,value } {} };
template<> struct Attr<void> : detail::NamedValue<void>
{ constexpr Attr(std::string_view name) : detail::NamedValue<void>{ name } {} };
template<size_t N> Attr(std::string_view, const char[N])->Attr<std::string_view>;
Attr(std::string_view)->Attr<void>;
template<typename... Attrs> struct AttrList : detail::BaseList<AttrList, Attrs...>
{ using detail::BaseList<AttrList, Attrs...>::BaseList; };
template<typename... Attrs> AttrList(Attrs...)->AttrList<Attrs...>;
template<typename... Attrs> AttrList(std::tuple<Attrs...>)->AttrList<Attrs...>;
template<typename U, typename V, bool s, bool f> struct FTraitsB {
	using object_type = U; using value_type = V;
	static constexpr bool is_static = s, is_function = f;
};
template<typename T> struct FTraits; // Field Traits
template<typename U, typename T> struct FTraits<T U::*> : FTraitsB<U, T, false, detail::IsFunc<T>::value> {};
template<typename T> struct FTraits<T*> : FTraitsB<void, T, true, detail::IsFunc<T>::value>{}; // static member
template<typename T> struct FTraits : FTraitsB<void, T, true, false> {}; // enum
template<typename T, typename AList> struct Field : FTraits<T>, detail::NamedValue<T> {
	AList attrs;
	constexpr Field(std::string_view n, T v, AList as) : detail::NamedValue<T>{n,v}, attrs{as} {}
};
template<typename T, typename AList> Field(std::string_view, T, AList)->Field<T, AList>;
template<typename... Fields> struct FieldList : detail::BaseList<FieldList, Fields...> {
	using detail::BaseList<FieldList, Fields...>::BaseList;
	template<typename TList> constexpr auto UnionTypeList(TList typeList) const
	{ return detail::FieldListUnionTypeList(*this, typeList, std::make_index_sequence<typeList.size>{}); }
};
template<typename... Fields> FieldList(Fields...)->FieldList<Fields...>;
template<typename... Fields> FieldList(std::tuple<Fields...>)->FieldList<Fields...>;
template<typename T> struct Type; // name, type, subclasses, fields, attrs
template<typename... Ts> struct TypeList : detail::BaseList<TypeList, Ts...>
{ using detail::BaseList<TypeList, Ts...>::BaseList; };
template<typename... Ts> TypeList(Ts...)->TypeList<Ts...>;
template<typename... Ts> TypeList(std::tuple<Ts...>)->TypeList<Ts...>;
template<typename T, typename Func> constexpr void ForEachVarOf(T&& obj, const Func& func) {
	Type<std::decay_t<T>>::fields.ForEach
	([&](auto field) { if constexpr (!field.is_static && !field.is_function) func(obj.*(field.value)); });
}