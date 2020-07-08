#pragma once

#include <tuple>
#include <string_view>

namespace Ubpa::USRefl::detail {
	template<size_t idx, typename T, typename Func>
	static constexpr void ForNonStaticFieldOf(T&& obj, const Func& func) {
		const auto& field = std::get<idx>(Type<std::decay_t<T>>::fields);
		if constexpr (!field.is_static)
			func(obj.*(field.value));
	}

	template<typename T, typename Func, size_t... Ns>
	static constexpr void ForEachVarOf(T&& obj, const Func& func, std::index_sequence<Ns...>) {
		(ForNonStaticFieldOf<Ns>(obj, func), ...);
	}

	template<typename List, typename Func, size_t... Ns>
	static constexpr void ForEach(const List& list, const Func& func, std::index_sequence<Ns...>) {
		(func(std::get<Ns>(list)), ...);
	}

	template<typename Indices>
	struct IndexSequenceTraits;
	template<size_t N0, size_t... Ns>
	struct IndexSequenceTraits<std::index_sequence<N0, Ns...>> {
		static constexpr size_t head = N0;
		using tail = std::index_sequence<Ns...>;
	};

	template<typename List, typename Func, size_t... Ns>
	static constexpr size_t FindIf(const List& list, const Func& func, std::index_sequence<Ns...>) {
		if constexpr (sizeof...(Ns) == 0)
			return static_cast<size_t>(-1);
		else {
			constexpr size_t head = IndexSequenceTraits<std::index_sequence<Ns...>>::head;
			using tail = typename IndexSequenceTraits<std::index_sequence<Ns...>>::tail;
			return func(std::get<head>(list)) ?
				head : FindIf(list, func, tail{});
		}
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
	struct BaseList : std::tuple<Elems...> {
		static constexpr size_t size = sizeof...(Elems);

		constexpr BaseList(Elems... elems) : std::tuple<Elems...>{ elems... } {}

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
			return std::get<N>(*this);
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

	template <typename... Args>
	constexpr Overload<Args...> overload_v{};

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
	template<size_t N> Attr(std::string_view, const char[N])->Attr<std::string_view>;
	Attr(std::string_view)->Attr<void>;
	template<typename T> struct IsAttr : std::false_type {};
	template<typename T> struct IsAttr<Attr<T>> : std::true_type {};

	template<typename... Attrs>
	struct AttrList : detail::BaseList<Attrs...> {
		static_assert((IsAttr<Attrs>::value&&...));
		using detail::BaseList<Attrs...>::BaseList;
	};
	template<typename T> struct IsAttrList : std::false_type {};
	template<typename... Attrs> struct IsAttrList<AttrList<Attrs...>> : std::true_type {};

	template<typename T, typename AList>
	struct Field : FieldTraits<T>, detail::NamedValue<T> {
		static_assert(IsAttrList<AList>::value);
		static_assert(!std::is_void_v<T>);

		constexpr Field(std::string_view name, T value, AList attrs)
			: detail::NamedValue<T>{name, value}, attrs{ attrs }{}

		AList attrs;
	};
	template<typename T, typename AList>
	Field(std::string_view, T, AList)->Field<T, AList>;
	template<typename T> struct IsField : std::false_type {};
	template<typename T, typename AList> struct IsField<Field<T, AList>> : std::true_type {};

	// Field's (name, value_type) must be unique
	template<typename... Fields>
	struct FieldList : detail::BaseList<Fields...> {
		static_assert((IsField<Fields>::value&&...));
		using detail::BaseList<Fields...>::BaseList;
	};


	// name, type, fields, attrs, subclasses
	template<typename T> struct Type;
	template<typename T> struct IsType : std::false_type {};
	template<typename T> struct IsType<Type<T>> : std::true_type {};

	template<typename... Types>
	struct TypeList : detail::BaseList<Types...> {
		using detail::BaseList<Types...>::BaseList;
		static_assert((IsType<Types>::value&&...));
	};

	// non-static member variables
	template<typename T, typename Func>
	constexpr void ForEachVarOf(T&& obj, const Func& func) {
		detail::ForEachVarOf(std::forward<T>(obj), func,
			std::make_index_sequence<Type<std::decay_t<T>>::fields.size>{});
	}
}
