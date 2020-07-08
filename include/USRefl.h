#pragma once

#include <tuple>
#include <array>
#include <string_view>

#define USREFL_FIELD_TYPE_OF(ptr) \
Ubpa::USRefl::Field<\
typename Ubpa::USRefl::FieldTraits<decltype(ptr)>::value_type,\
typename Ubpa::USRefl::FieldTraits<decltype(ptr)>::object_type,\
ptr>

#define USREFL_FIELD_OF(ptr) USREFL_FIELD_TYPE_OF(ptr){}

namespace Ubpa::USRefl {
	template<typename T>
	struct FieldTraits;
	template<typename T, typename Object>
	struct FieldTraits<T Object::*> {
		using value_type = T;
		using object_type = Object;
	};

	// name = "..."
	// metas[n] = {{"...", "..."}, ...}
	template<typename T>
	struct TypeDecl;
	// fields = {USReflField, ...}
	template<typename T, typename Object, T Object::* ptr>
	struct FieldDecl;

	template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
	template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

	struct Meta {
		constexpr Meta(std::string_view key, std::string_view value)
			:key{ key }, value{ value }{}
		constexpr Meta(std::string_view key) : Meta{ key,"" } {}
		constexpr Meta() : Meta{ "","" } {}
		std::string_view key;
		std::string_view value;
	};

	template<typename T_, typename Object, T_ Object::* ptr_>
	struct Field {
		using type = T_;
		static constexpr std::string_view name = FieldDecl<T_, Object, ptr_>::name;
		static constexpr auto ptr{ ptr_ };
		static constexpr auto metas = FieldDecl<T_, Object, ptr_>::metas;

		template<typename Func>
		static constexpr void ForEachMeta(const Func& func) {
			for (const auto& meta : metas)
				func(meta);
		}
	};

	template<typename T>
	struct Type {
		static constexpr auto fields = TypeDecl<T>::fields;
		static constexpr auto metas = TypeDecl<T>::metas;
		static constexpr size_t numFields = std::tuple_size_v<decltype(fields)>;

		template<typename Func>
		static constexpr void ForEachField(const Func& func) {
			ForEachField(func, std::make_index_sequence<numFields>{});
		}

		template<typename Func, size_t... Ns>
		static constexpr void ForEachField(const Func& func, std::index_sequence<Ns...>) {
			(func(std::get<Ns>(fields)), ...);
		}

		template<typename Func>
		static constexpr void ForEachMeta(const Func& func) {
			for (const auto& meta : metas)
				func(meta);
		}
	};

	template<typename T, typename Func>
	static constexpr void ForEachFieldOf(T&& obj, const Func& func) {
		ForEachFieldOf(std::forward<T>(obj), func, std::make_index_sequence<Type<std::decay_t<T>>::numFields>{});
	}

	template<typename T, typename Func, size_t... Ns>
	static constexpr void ForEachFieldOf(T&& obj, const Func& func, std::index_sequence<Ns...>) {
		(func(obj.*(std::get<Ns>(Type<std::decay_t<T>>::fields).ptr)), ...);
	}
}
