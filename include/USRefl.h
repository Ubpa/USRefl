#pragma once

#include <tuple>
#include <array>
#include <string_view>

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

	template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
	template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

	struct Meta {
		constexpr Meta(std::string_view key, std::string_view value)
			noexcept : key{ key }, value{ value }{}
		constexpr Meta(std::string_view key) noexcept : Meta{ key,"" } {}
		constexpr Meta() noexcept : Meta{ "","" } {}
		std::string_view key;
		std::string_view value;
	};

	template<typename T, typename Object, size_t N>
	struct Field {
		constexpr Field(
			std::string_view name,
			T Object::* ptr,
			std::array<Meta, N> metas
		) noexcept : name{ name }, ptr{ ptr }, metas{ metas }{}

		std::string_view name;
		T Object::* ptr;
		std::array<Meta, N> metas;

		template<typename Func>
		constexpr void ForEachMeta(const Func& func) const {
			for (const Meta& meta : metas)
				func(meta);
		}
	};

	template<typename T, typename Object, size_t N>
	constexpr auto GenField(std::string_view name, T Object::* ptr, std::array<Meta, N> metas) noexcept {
		return Field<T, Object, N>{ name, ptr, metas };
	}

	template<typename T>
	struct Type : TypeDecl<T> {
		static constexpr auto num_fields = std::tuple_size_v<decltype(fields)>;

		template<typename Func>
		static constexpr void ForEachField(const Func& func) {
			ForEachField(func, std::make_index_sequence<num_fields>{});
		}

		template<typename Func, size_t... Ns>
		static constexpr void ForEachField(const Func& func, std::index_sequence<Ns...>) {
			(func(std::get<Ns>(fields)), ...);
		}

		template<typename Func>
		static constexpr void ForEachMeta(const Func& func) {
			for (const Meta& meta : metas)
				func(meta);
		}
	};

	template<typename T, typename Func>
	static constexpr void ForEachFieldOf(T&& obj, const Func& func) {
		ForEachFieldOf(std::forward<T>(obj), func, std::make_index_sequence<Type<std::decay_t<T>>::num_fields>{});
	}

	template<typename T, typename Func, size_t... Ns>
	static constexpr void ForEachFieldOf(T&& obj, const Func& func, std::index_sequence<Ns...>) {
		(func(obj.*(std::get<Ns>(Type<std::decay_t<T>>::fields).ptr)), ...);
	}
}
