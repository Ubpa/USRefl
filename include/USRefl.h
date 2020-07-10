#pragma once

#include <tuple>
#include <string_view>

namespace Ubpa::USRefl::detail {
	template<typename T, template<typename...>class U>
	struct IsInstance : std::false_type {};
	template<template<typename...>class U, typename... Ts>
	struct IsInstance<U<Ts...>, U> : std::true_type {};

	template<typename Indices> struct IndexSequenceTraits;
	template<size_t N0, size_t... Ns>
	struct IndexSequenceTraits<std::index_sequence<N0, Ns...>> {
		static constexpr size_t head = N0;
		static constexpr auto tail = std::index_sequence<Ns...>{};
	};

	template<typename List, typename Func, size_t... Ns>
	constexpr void ForEach(const List& list, const Func& func, std::index_sequence<Ns...>) {
		(func(list.Get<Ns>()), ...);
	}

	template<typename List, typename Func, size_t... Ns>
	constexpr size_t FindIf(const List& list, const Func& func, std::index_sequence<Ns...>) {
		if constexpr (sizeof...(Ns) > 0) {
			using IST = IndexSequenceTraits<std::index_sequence<Ns...>>;
			return func(list.Get<IST::head>()) ? IST::head : FindIf(list, func, IST::tail);
		}
		else return static_cast<size_t>(-1);
	}

	template<typename T>
	struct NamedValueBase {
		std::string_view name;
		static constexpr bool has_value = !std::is_void_v<T>;

		template<typename U>
		static constexpr bool ValueTypeIs() {
			return std::is_same_v<T, U>;
		}

		template<typename U>
		static constexpr bool ValueTypeIsSameWith(U u) {
			return ValueTypeIs<U>();
		}
	};

	template<typename T>
	struct NamedValue : NamedValueBase<T> {
		constexpr NamedValue(std::string_view name, T value)
			: NamedValueBase<T>{ name }, value{ value } {}
		T value;
	};
	template<>
	struct NamedValue<void> : NamedValueBase<void> {
		constexpr NamedValue(std::string_view name)
			: NamedValueBase<void>{ name } {}
	};

	// Elems has name
	template<typename... Elems>
	struct BaseList {
		std::tuple<Elems...> list;
		static constexpr size_t size = sizeof...(Elems);

		constexpr BaseList(Elems... elems) : list{ elems... } {}
		constexpr BaseList(std::tuple<Elems...> elems) : list{ elems } {}

		template<typename Func>
		constexpr void ForEach(const Func& func) const {
			detail::ForEach(*this, func, std::make_index_sequence<size>{});
		}

		template<typename Func>
		constexpr size_t FindIf(const Func& func) const {
			return detail::FindIf(*this, func, std::make_index_sequence<size>{});
		}

		constexpr size_t Find(std::string_view name) const {
			return FindIf([name](auto elem) { return elem.name == name; });
		}
		
		template<typename T>
		constexpr size_t FindByValue(T value) const {
			return FindIf([value](auto elem) {
				if constexpr (elem.has_value) {
					if constexpr (elem.ValueTypeIs<T>())
						return elem.value == value;
					else
						return false;
				}
				else
					return false;
			});
		}

		constexpr bool Contains(std::string_view name) const {
			return Find(name) != static_cast<size_t>(-1);
		}

		template<size_t N>
		constexpr auto Get() const {
			static_assert(N != static_cast<size_t>(-1));
			return std::get<N>(list);
		}
	};
}

namespace Ubpa::USRefl {
	// field : member variable/function, static or non-static

	// Ret(Args...)[const]
	template<typename Func>
	struct IsFunc : std::false_type {};
	template<typename Ret, typename... Args>
	struct IsFunc<Ret(Args...)> : std::true_type {};
	template<typename Ret, typename... Args>
	struct IsFunc<Ret(Args...)const> : std::true_type {};
	template<typename Func>
	static constexpr bool IsFunc_v = IsFunc<Func>::value;

	template <typename... Args>
	struct Overload {
		template <typename R, typename T>
		constexpr auto operator()(R(T::* func_ptr)(Args...)) const {
			return func_ptr;
		}
		template <typename R, typename T>
		constexpr auto operator()(R(T::* func_ptr)(Args...) const) const {
			return func_ptr;
		}
		template <typename R>
		constexpr auto operator()(R(*func_ptr)(Args...)) const {
			return func_ptr;
		}
	};

	template <typename... Args> constexpr Overload<Args...> overload_v{};

	template<typename T>
	struct FieldTraits;
	template<typename Object, typename T>
	struct FieldTraits<T Object::*> {
		using object_type = Object;
		using value_type = T;
		static constexpr bool is_static = false;
		static constexpr bool is_function = IsFunc_v<T>;
	};
	template<typename T>
	struct FieldTraits<T*> {
		using object_type = void;
		using value_type = T;
		static constexpr bool is_static = true;
		static constexpr bool is_function = IsFunc_v<T>;
	};
	template<typename T>
	struct FieldTraits {
		static_assert(std::is_enum_v<T>);
		using object_type = void;
		using value_type = T;
		static constexpr bool is_static = true;
		static constexpr bool is_function = false;
	};

	template<typename T>
	struct Attr : detail::NamedValue<T> {
		constexpr Attr(std::string_view name, T value) : detail::NamedValue<T>{ name,value } {}
	};
	template<>
	struct Attr<void> : detail::NamedValue<void> {
		constexpr Attr(std::string_view name) : detail::NamedValue<void>{ name } {}
	};
	template<size_t N>
	Attr(std::string_view, const char[N])->Attr<std::string_view>;
	Attr(std::string_view)->Attr<void>;

	template<typename... Attrs>
	struct AttrList : detail::BaseList<Attrs...> {
		static_assert((detail::IsInstance<Attrs, Attr>::value&&...));
		using detail::BaseList<Attrs...>::BaseList;
	};
	template<typename... Attrs> AttrList(Attrs...)->AttrList<Attrs...>;
	template<typename... Attrs> AttrList(std::tuple<Attrs...>)->AttrList<Attrs...>;

	template<typename T, typename AList>
	struct Field : FieldTraits<T>, detail::NamedValue<T> {
		static_assert(detail::IsInstance<AList, AttrList>::value);
		static_assert(!std::is_void_v<T>);

		constexpr Field(std::string_view name, T value, AList attrs)
			: detail::NamedValue<T>{name, value}, attrs{ attrs }{}

		AList attrs;
	};
	template<typename T, typename AList>
	Field(std::string_view, T, AList)->Field<T, AList>;

	template<typename... Types> struct TypeList; // forward declaration

	// Field's (name, value_type) must be unique
	template<typename... Fields>
	struct FieldList : detail::BaseList<Fields...> {
		static_assert((detail::IsInstance<Fields, Field>::value&&...));
		using detail::BaseList<Fields...>::BaseList;
	};
	template<typename... Fields> FieldList(Fields...)->FieldList<Fields...>;
	template<typename... Fields> FieldList(std::tuple<Fields...>)->FieldList<Fields...>;

	// name, type, bases, fields, attrs, 
	template<typename T> struct Type;

	template<typename... Types>
	struct TypeList : detail::BaseList<Types...> {
		static_assert((detail::IsInstance<Types, Type>::value&&...));
		using detail::BaseList<Types...>::BaseList;
	};
	template<typename... Types> TypeList(Types...)->TypeList<Types...>;
	template<typename... Types> TypeList(std::tuple<Types...>)->TypeList<Types...>;

	// non-static member variables
	template<typename T, typename Func>
	constexpr void ForEachVarOf(T&& obj, const Func& func) {
		Type<std::decay_t<T>>::fields.ForEach ([&](auto field) {
			if constexpr (!field.is_static && !field.is_function) func(obj.*(field.value));
		});
	}
}
