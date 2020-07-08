#pragma once

#include <tuple>
#include <string_view>

namespace Ubpa::USRefl::detail {
	template<size_t idx, typename T, typename Func>
	static constexpr void ForNonStaticFieldOf(T&& obj, const Func& func) {
		const auto& field = std::get<idx>(Type<std::decay_t<T>>::fields);
		if constexpr (!field.is_static)
			func(obj.*(field.ptr));
	}

	template<typename T, typename Func, size_t... Ns>
	static constexpr void ForEachVarOf(T&& obj, const Func& func, std::index_sequence<Ns...>) {
		(ForNonStaticFieldOf<Ns>(obj, func), ...);
	}

	template<typename Fields, typename Func, size_t... Ns>
	static constexpr void ForEachField(const Fields& fields, const Func& func, std::index_sequence<Ns...>) {
		(func(std::get<Ns>(fields)), ...);
	}

	template<typename AttrList, typename Func, size_t... Ns>
	static constexpr void ForEachAttr(const AttrList& attrs, const Func& func, std::index_sequence<Ns...>) {
		(func(std::get<Ns>(attrs)), ...);
	}

	template<typename Indices>
	struct IndexSequenceTraits;
	template<size_t N0, size_t... Ns>
	struct IndexSequenceTraits<std::index_sequence<N0, Ns...>> {
		static constexpr size_t head = N0;
		using tail = std::index_sequence<Ns...>;
	};

	template<typename FieldList, typename Func, size_t... Ns>
	static constexpr size_t FindIfField(const FieldList& fields, const Func& func, std::index_sequence<Ns...>) {
		if constexpr (sizeof...(Ns) == 0)
			return static_cast<size_t>(-1);
		else {
			constexpr size_t head = IndexSequenceTraits<std::index_sequence<Ns...>>::head;
			using tail = typename IndexSequenceTraits<std::index_sequence<Ns...>>::tail;
			return func(std::get<head>(fields)) ?
				head : FindIfField(fields, func, tail{});
		}
	}
	
	template<typename AttrList, typename Func, size_t... Ns>
	static constexpr size_t FindIfAttr(const AttrList& attrs, const Func& func, std::index_sequence<Ns...>) {
		if constexpr (sizeof...(Ns) == 0)
			return false;
		else {
			constexpr size_t head = IndexSequenceTraits<std::index_sequence<Ns...>>::head;
			using tail = typename IndexSequenceTraits<std::index_sequence<Ns...>>::tail;
			return func(std::get<head>(attrs)) ?
				head : FindIfAttr(attrs, func, tail{});
		}
	}
}

namespace Ubpa::USRefl {
	// field : member variable/function, static or non-static

	// fields
	// attrs
	template<typename T>
	struct Type;

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
		constexpr auto operator()(R(T::* ptr)(Args...)) const {
			return ptr;
		}
		template <typename R, typename T>
		constexpr auto operator()(R(T::* ptr)(Args...) const) const {
			return ptr;
		}
		template <typename R>
		constexpr auto operator()(R(*ptr)(Args...)) const {
			return ptr;
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

	// name = "..."
	// metas[n] = {{"...", "..."}, ...}
	template<typename T>
	struct TypeDecl;

	struct EmptyType {};

	template<typename T = EmptyType>
	struct Attr {
		constexpr Attr(std::string_view key, T value)  : key{ key }, value{ value }{}
		constexpr Attr(std::string_view key)  : Attr{ key,T{} } {}
		constexpr Attr()  : Attr{ "",T{} } {}
		std::string_view key;
		T value;
		static constexpr bool is_value_empty = std::is_same_v<T, EmptyType>;
	};
	using AttrStr = Attr<std::string_view>;
	template<size_t N>
	Attr(std::string_view, const char[N])->Attr<std::string_view>;

	template<typename... Attrs>
	struct AttrList : std::tuple<Attrs...> {
		static constexpr size_t num_attrs = sizeof...(Attrs);

		constexpr AttrList(Attrs... attrs) : std::tuple<Attrs...>{ attrs... } {}

		template<typename Func>
		constexpr void ForEach(const Func& func) const {
			detail::ForEachAttr(*this, func, std::make_index_sequence<num_attrs>{});
		}

		template<typename Func>
		constexpr size_t FindIf(const Func& func) const {
			return detail::FindIfAttr(*this, func, std::make_index_sequence<num_attrs>{});
		}

		constexpr size_t Find(std::string_view name) const {
			return FindIf([key](auto attr) { return attr.key == key; });
		}

		constexpr bool Contains(std::string_view key) const {
			return Find(key) != static_cast<size_t>(-1);
		}

		template<size_t N>
		constexpr auto Get() const  {
			static_assert(N != static_cast<size_t>(-1));
			return std::get<N>(*this);
		}
	};
	template<class... Attrs> AttrList(Attrs...)->AttrList<Attrs...>;

	template<typename T, typename AList>
	struct Field : FieldTraits<T> {
		constexpr Field(
			std::string_view name,
			T ptr,
			AList attrs
		)  : name{ name }, ptr{ ptr }, attrs{ attrs }{}

		std::string_view name;
		T ptr;
		AList attrs;

		template<typename U>
		static constexpr bool ValueTypeIs() {
			return std::is_same_v<T, U>;
		}

		template<typename U>
		static constexpr bool ValueTypeIsSameWith(U u) {
			return ValueTypeIs<U>();
		}
	};
	template<typename T, typename AList>
	Field(std::string_view, T, AList)->Field<T, AList>;

	// Field's (name, value_type) must be unique
	template<typename... Fields>
	struct FieldList : std::tuple<Fields...> {
		static constexpr size_t num_fields = sizeof...(Fields);

		constexpr FieldList(Fields... fields) : std::tuple<Fields...>{ fields... } {}

		template<typename Func>
		constexpr void ForEach(const Func& func) const {
			detail::ForEachField(*this, func, std::make_index_sequence<num_fields>{});
		}

		template<typename Func>
		constexpr size_t FindIf(const Func& func) const {
			return detail::FindIfField(*this, func, std::make_index_sequence<num_fields>{});
		}

		constexpr size_t Find(std::string_view name) const {
			return FindIf([name](auto field) { return field.name == name; });
		}
		
		template<typename T>
		constexpr size_t Find(std::string_view name, const T& value) const {
			return FindIf([name, value](auto field) {
				if constexpr (std::is_same_v<std::decay_t<T>, typename field::value_type>)
					return field.name == name && field.value == value;
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
	template<class... Fields> FieldList(Fields...)->FieldList<Fields...>;

	// non-static member variables
	template<typename T, typename Func>
	constexpr void ForEachVarOf(T&& obj, const Func& func) {
		detail::ForEachVarOf(std::forward<T>(obj), func,
			std::make_index_sequence<Type<std::decay_t<T>>::fields.num_fields>{});
	}
}
