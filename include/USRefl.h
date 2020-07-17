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

	template<typename List, typename Func, typename Acc, size_t... Ns>
	constexpr auto Accumulate(const List& list, Func&& func, Acc&& acc, std::index_sequence<Ns...>) {
		if constexpr (sizeof...(Ns) > 0) {
			using IST = IndexSequenceTraits<std::index_sequence<Ns...>>;
			return Accumulate(list, std::forward<Func>(func), func(std::forward<Acc>(acc), list.template Get<IST::head>()), IST::tail);
		}
		else return acc;
	}

	template<typename List, typename Func, size_t... Ns>
	constexpr size_t FindIf(const List& list, Func&& func, std::index_sequence<Ns...>) {
		if constexpr (sizeof...(Ns) > 0) {
			using IST = IndexSequenceTraits<std::index_sequence<Ns...>>;
			return func(list.template Get<IST::head>()) ? IST::head : FindIf(list, std::forward<Func>(func), IST::tail);
		}
		else return static_cast<size_t>(-1);
	}

	template<typename TI, typename U, typename Func>
	static constexpr void ForEachNonVirtualVarOf(TI info, U&& obj, Func&& func) {
		info.fields.ForEach([&](auto field) {
			if constexpr (!field.is_static && !field.is_func)
				std::forward<Func>(func)(std::forward<U>(obj).*(field.value));
		});
		info.bases.ForEach([&](auto base) {
			if constexpr (!base.is_virtual)
				ForEachNonVirtualVarOf(base.info, base.info.Forward(std::forward<U>(obj)), std::forward<Func>(func));
		});
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
	struct ElemList {
		std::tuple<Elems...> list;
		static constexpr size_t size = sizeof...(Elems);

		constexpr ElemList(Elems... elems) : list{ elems... } {}

		template<typename Func, typename Acc>
		constexpr auto Accumulate(Acc&& acc, Func&& func) const {
			return detail::Accumulate(*this, std::forward<Func>(func), std::forward<Acc>(acc), std::make_index_sequence<size>{});
		}

		template<typename Func>
		constexpr void ForEach(Func&& func) const {
			std::apply([&](auto... elems) { (std::forward<Func>(func)(elems), ...); }, list);
		}

		template<typename Func>
		constexpr size_t FindIf(Func&& func) const {
			return detail::FindIf(*this, std::forward<Func>(func), std::make_index_sequence<size>{});
		}

		constexpr size_t Find(std::string_view name) const {
			return FindIf([name](auto elem) { return elem.name == name; });
		}
		
		template<typename T>
		constexpr size_t FindValue(T value) const {
			return FindIf([value](auto e) {
				if constexpr (e.has_value) {
					if constexpr (std::is_same_v<decltype(e.value), T>)
						return e.value == value;
					else
						return false;
				}
				else return false;
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

		template<typename Elem>
		constexpr auto UniqueInsert(Elem e) const {
			if constexpr ((std::is_same_v<Elems, Elem> || ...))
				return *this;
			else {
				return std::apply([e](auto... elems) {
					return ElemList<Elems..., Elem>{ elems..., e };
				}, list);
			}
		}

// name must be constexpr std::string_view
// C++20 support string literal as template arguments
#define USRefl_ElemList_GetByName(list, name) list.Get<list.Find(name)>()

// value must be constexpr
// C++20 support string literal as template arguments
#define USRefl_ElemList_GetByValue(list, value) list.Get<list.FindValue(value)>()
	};
}

namespace Ubpa::USRefl {
	// field : member variable/function, static or non-static

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
		static constexpr bool is_func = std::is_function_v<T>;
	};
	template<typename T>
	struct FieldTraits<T*> {
		using object_type = void;
		using value_type = T;
		static constexpr bool is_static = true;
		static constexpr bool is_func = std::is_function_v<T>;
	};
	template<typename T>
	struct FieldTraits {
		static_assert(std::is_enum_v<T>);
		using object_type = void;
		using value_type = T;
		static constexpr bool is_static = true;
		static constexpr bool is_func = false;
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
	Attr(std::string_view, const char(&)[N])->Attr<std::string_view>;
	Attr(std::string_view)->Attr<void>;

	template<typename... Attrs>
	struct AttrList : detail::ElemList<Attrs...> {
		static_assert((detail::IsInstance<Attrs, Attr>::value&&...));
		constexpr AttrList(Attrs... attrs) : detail::ElemList<Attrs...>{ attrs... } {}
	};

	template<typename T, typename AList>
	struct Field : FieldTraits<T>, detail::NamedValue<T> {
		static_assert(detail::IsInstance<AList, AttrList>::value);
		static_assert(!std::is_void_v<T>);

		constexpr Field(std::string_view name, T value, AList attrs = {})
			: detail::NamedValue<T>{name, value}, attrs{ attrs }{}

		AList attrs;
	};
	template<typename T, typename AList>
	Field(std::string_view, T, AList)->Field<T, AList>;
	template<typename T>
	Field(std::string_view, T)->Field<T, AttrList<>>;

	template<typename... TypeInfos> struct TypeInfoList; // forward declaration

	// Field's (name, value_type) must be unique
	template<typename... Fields>
	struct FieldList : detail::ElemList<Fields...> {
		static_assert((detail::IsInstance<Fields, Field>::value&&...));
		constexpr FieldList(Fields... fields) : detail::ElemList<Fields...>{ fields... } {};
	};

	// name, type, bases, fields, attrs, 
	template<typename T> struct TypeInfo;

	template<typename T, bool IsVirtual = false>
	struct Base {
		static constexpr auto info = TypeInfo<T>{};
		static constexpr bool is_virtual = IsVirtual;
	};

	template<typename... Bases>
	struct BaseList : detail::ElemList<Bases...> {
		// static_assert((detail::IsInstance<Bases, Base>::value&&...));
		constexpr BaseList(Bases... bases) : detail::ElemList<Bases...>{ bases... } {};
	};

	template<typename T, typename... Bases>
	struct TypeInfoBase {
		using type = T;
		static constexpr BaseList bases = { Bases{}... };

		template<typename Derived>
		static constexpr auto&& Forward(Derived&& derived) noexcept {
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

		static constexpr auto GetVirtualBases() {
			return bases.Accumulate(detail::ElemList<>{}, [](auto acc, auto base) {
				constexpr auto vbs = base.info.GetVirtualBases();
				auto concated = vbs.Accumulate(acc, [](auto acc, auto vb){
					return acc.UniqueInsert(vb);
				});
				if constexpr (base.is_virtual)
					return concated.UniqueInsert(base.info);
				else
					return concated;
			});
		}

		template<typename Func, size_t Depth = 0>
		static constexpr void DFS(Func&& func) {
			func(TypeInfo<type>{}, Depth);
			if constexpr (Depth == 0) {
				GetVirtualBases().ForEach([&](auto vb) {
					func(vb, 1);
				});
			}
			bases.ForEach([&](auto base) {
				if constexpr (!base.is_virtual)
					base.info.template DFS<Func, Depth + 1>(std::forward<Func>(func));
			});
		}

		template<typename U, typename Func>
		static constexpr void ForEachVarOf(U&& obj, Func&& func) {
			static_assert(std::is_same_v<type, std::decay_t<U>>);
			GetVirtualBases().ForEach([&](auto vb) {
				vb.fields.ForEach([&](auto field) {
					if constexpr (!field.is_static && !field.is_func)
						std::forward<Func>(func)(std::forward<U>(obj).*(field.value));
				});
			});
			detail::ForEachNonVirtualVarOf(TypeInfo<type>{}, std::forward<U>(obj), std::forward<Func>(func));
		}
	};
}
