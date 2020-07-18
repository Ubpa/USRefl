#pragma once                           // Ubpa Static Reflection -- 99 lines
#include <string_view>                 // Repository: https://github.com/Ubpa/USRefl
#include <tuple>                       // License: https://github.com/Ubpa/USRefl/blob/master/LICENSE
namespace Ubpa::USRefl::detail {
  template<typename L, typename F> constexpr size_t FindIf(L, F&&, std::index_sequence<>) { return -1; }
  template<typename L, typename F, size_t N0, size_t... Ns>
  constexpr size_t FindIf(L l, F&& f, std::index_sequence<N0, Ns...>)
  { return f(l.template Get<N0>()) ? N0 : FindIf(l, std::forward<F>(f), std::index_sequence<Ns...>{}); }
  template<typename L, typename F, typename R> constexpr auto Acc(L, F&&, R&& r, std::index_sequence<>) { return r; }
  template<typename L, typename F, typename R, size_t N0, size_t... Ns>
  constexpr auto Acc(L l, F&& f, R&& r, std::index_sequence<N0, Ns...>)
  { return Acc(l, std::forward<F>(f), f(std::forward<R>(r), l.template Get<N0>()), std::index_sequence<Ns...>{}); }
  template<typename TI, typename U, typename F> constexpr void NV_Var(TI info, U&& obj, F&& f) {
    info.fields.ForEach([&](auto fld)
    { if constexpr (!fld.is_static && !fld.is_func) std::forward<F>(f)(std::forward<U>(obj).*(fld.value)); });
    info.bases.ForEach([&](auto b)
    { if constexpr (!b.is_virtual) NV_Var(b.info, b.info.Forward(std::forward<U>(obj)), std::forward<F>(f)); });
  }
}
namespace Ubpa::USRefl {
  template<typename T> struct NamedValue { // named value
    std::string_view name; T value;
    static constexpr bool has_value = true;
    template<typename U>constexpr bool operator==(U v)const{if constexpr(std::is_same_v<T,U>)return value==v;else return false;}
  };
  template<> struct NamedValue<void> {
    std::string_view name; /*T value;*/
    static constexpr bool has_value = false;
    template<typename U> constexpr bool operator==(U) const { return false; }
  };
  template<typename...Elems> struct ElemList { // Elems is a named value
    std::tuple<Elems...> elems;
    constexpr ElemList(Elems... elems) : elems{ elems... } {}
    template<typename Func> constexpr void ForEach(Func&& func) const
    { std::apply([&](auto... elems) { (std::forward<Func>(func)(elems), ...); }, elems); }
    template<typename Func, typename Init> constexpr auto Accumulate(Init&& init, Func&& func) const
    { return detail::Acc(*this, std::forward<Func>(func), std::forward<Init>(init), std::make_index_sequence<sizeof...(Elems)>{}); }
    template<typename Func> constexpr size_t FindIf(Func&& func) const
    { return detail::FindIf(*this, std::forward<Func>(func), std::make_index_sequence<sizeof...(Elems)>{}); }
    constexpr size_t Find(std::string_view n) const { return FindIf([n](auto e){return e.name == n;}); }
    template<typename T> constexpr size_t FindValue(T v) const { return FindIf([v](auto e) { return e == v; }); }
    constexpr bool Contains(std::string_view name) const { return Find(name) != static_cast<size_t>(-1); }
    template<typename Elem> constexpr auto UniqueInsert(Elem e) const {
      if constexpr ((std::is_same_v<Elems, Elem> || ...)) return *this;
      else return std::apply([e](auto... es) { return ElemList<Elems..., Elem>{ es..., e }; }, elems);
    }
    template<size_t N> constexpr auto Get() const { return std::get<N>(elems); }
  #define USRefl_ElemList_GetByName(list, name) list.Get<list.Find(name)>()
  #define USRefl_ElemList_GetByValue(list, value) list.Get<list.FindValue(value)>()
  };
  template<typename T>struct Attr : NamedValue<T> { constexpr Attr(std::string_view n, T v) : NamedValue<T>{ n,v } {} };
  template<> struct Attr<void> : NamedValue<void> { constexpr Attr(std::string_view n) : NamedValue<void>{ n } {} };
  template<typename...As>struct AttrList : ElemList<As...> { constexpr AttrList(As...as) : ElemList<As...>{ as... } {} };
  template<bool s, bool f> struct FTraitsB { static constexpr bool is_static = s, is_func = f; };
  template<typename T> struct FTraits : FTraitsB<true, false> {}; // default is enum
  template<typename U, typename T> struct FTraits<T U::*> : FTraitsB<false, std::is_function_v<T>> {};
  template<typename T> struct FTraits<T*> : FTraitsB<true, std::is_function_v<T>>{}; // static member
  template<typename T, typename AList> struct Field : FTraits<T>, NamedValue<T> {
    AList attrs;
    constexpr Field(std::string_view n, T v, AList as = {}) : NamedValue<T>{ n,v }, attrs{ as } {}
  };
  template<typename...Fs>struct FieldList :ElemList<Fs...> { constexpr FieldList(Fs...fs) :ElemList<Fs...>{ fs... } {} };
  template<typename T> struct TypeInfo; // TypeInfoBase, name, fields, attrs
  template<typename T, bool IsVirtual = false> struct Base {
    static constexpr auto info = TypeInfo<T>{};
    static constexpr bool is_virtual = IsVirtual;
  };
  template<typename...Bs> struct BaseList : ElemList<Bs...> { constexpr BaseList(Bs... bs) : ElemList<Bs...>{ bs... } {} };
  template<typename...Ts> struct TypeInfoList : ElemList<Ts...> { constexpr TypeInfoList(Ts...ts) : ElemList<Ts...>{ ts... } {} };
  template<typename T, typename... Bases> struct TypeInfoBase {
    using type = T;
    static constexpr BaseList bases = { Bases{}... };
    template<typename U> static constexpr auto&& Forward(U&& derived) noexcept {
      if constexpr (std::is_same_v<std::decay_t<U>, U>) return static_cast<type&&>(derived); // right
      else if constexpr (std::is_same_v<std::decay_t<U>&, U>) return static_cast<type&>(derived); // left
      else return static_cast<const std::decay_t<U>&>(derived); // std::is_same_v<const std::decay_t<U>&, U>
    }
    static constexpr auto VirtualBases() {
      return bases.Accumulate(ElemList<>{}, [](auto acc, auto base) {
        auto concated = base.info.VirtualBases().Accumulate(acc, [](auto acc, auto b) { return acc.UniqueInsert(b); });
        if constexpr (!base.is_virtual) return concated; else return concated.UniqueInsert(base.info);
      });
    }
    template<typename F, size_t Depth = 0> static constexpr void DFS(F&& f) {
      f(TypeInfo<type>{}, Depth);
      if constexpr (Depth == 0) VirtualBases().ForEach([&](auto vb) { std::forward<F>(f)(vb, 1); });
      bases.ForEach([&](auto b) { if constexpr (!b.is_virtual) b.info.template DFS<F, Depth + 1>(std::forward<F>(f)); });
    }
    template<typename U, typename Func> static constexpr void ForEachVarOf(U&& obj, Func&& func) {
      VirtualBases().ForEach([&](auto vb) { vb.fields.ForEach([&](auto fld)
      { if constexpr (!fld.is_static && !fld.is_func) std::forward<Func>(func)(std::forward<U>(obj).*(fld.value)); }); });
      detail::NV_Var(TypeInfo<type>{}, std::forward<U>(obj), std::forward<Func>(func));
    }
  };
  template<size_t N> Attr(std::string_view, const char(&)[N])->Attr<std::string_view>;
  Attr(std::string_view)->Attr<void>;
  template<typename T, typename AList> Field(std::string_view, T, AList)->Field<T, AList>;
  template<typename T> Field(std::string_view, T)->Field<T, AttrList<>>;
}