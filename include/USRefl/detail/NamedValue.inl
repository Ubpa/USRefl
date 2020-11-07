#pragma once

#include <utility>
#include <string_view>

namespace Ubpa::USRefl::detail {
	template<typename Str>
    struct USTRSizeof;
    template<typename Char>
    struct USTRSizeof<const std::basic_string_view<Char>&> {
        static constexpr size_t get(const std::basic_string_view<Char>& str) noexcept {
            return str.size();
        }
    };
    template<typename Char, size_t N>
    struct USTRSizeof<const Char(&)[N]> {
        static constexpr size_t get(const Char(&str)[N]) noexcept {
            return N - 1;
        }
    };

    template <typename Char, typename T, size_t ...N>
    constexpr decltype(auto) USTRHelperImpl(std::index_sequence<N...>) {
        return std::integer_sequence<Char, T::get()[N]...>();
    }

    template <typename T>
    constexpr decltype(auto) USTRHelper(T) {
        using Char = std::decay_t<decltype(T::get()[0])>;
        return USTRHelperImpl<Char, T>(std::make_index_sequence<USTRSizeof<decltype(T::get())>::template get(T::get())>());
    }
}
