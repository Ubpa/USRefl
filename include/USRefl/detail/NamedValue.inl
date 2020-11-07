#pragma once

#include <utility>

namespace Ubpa::USRefl::detail {
    template <typename Char, typename T, size_t ...N>
    constexpr decltype(auto) USTRHelperImpl(std::index_sequence<N...>) {
        return std::integer_sequence<Char, T::get()[N]...>();
    }

    template <typename T>
    constexpr decltype(auto) USTRHelper(T) {
        using Char = std::decay_t<decltype(T::get()[0])>;
        return USTRHelperImpl<Char, T>(std::make_index_sequence<sizeof(T::get()) / sizeof(Char) - 1>());
    }
}
