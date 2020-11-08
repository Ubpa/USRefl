#pragma once                           // Ubpa Static Reflection -- 99 lines
#include <string_view>                 // Repository: https://github.com/Ubpa/USRefl
#include <tuple>                       // License: https://github.com/Ubpa/USRefl/blob/master/LICENSE
#define TSTR(s) Ubpa::USRefl::detail::TNameImpl1([] { struct tmp { static constexpr decltype(auto) get() { return (s); } }; return tmp{}; }())
namespace Ubpa::USRefl::detail {
  template<class C, C... cs>struct TStr{using Tag=TStr; template<class T>static constexpr bool NameIs(T={}){return std::is_same_v<T,Tag>;}
    using Char=C; static constexpr char name_data[]{cs...,C(0)}; static constexpr std::string_view name{name_data}; };
  template<class Char,class T,size_t...N>constexpr auto TNameImpl2(std::index_sequence<N...>) { return TStr<Char, T::get()[N]...>(); }
  template <typename T> constexpr auto TNameImpl1(T) { using Char = std::decay_t<decltype(T::get()[0])>;
    return TNameImpl2<Char, T>(std::make_index_sequence<sizeof(T::get()) / sizeof(Char) - 1>()); }
  template<class L, class F> constexpr size_t FindIf(L, F&&, std::index_sequence<>) { return -1; }
  template<class L, class F, size_t N0, size_t... Ns> constexpr size_t FindIf(L l, F&& f, std::index_sequence<N0, Ns...>)
  { return f(l.template Get<N0>()) ? N0 : FindIf(l, std::forward<F>(f), std::index_sequence<Ns...>{}); }
  template<class L, class F, class R> constexpr auto Acc(L, F&&, R&& r, std::index_sequence<>) { return std::forward<R>(r); }
  template<bool m0, bool... ms, class L, class F, class R, size_t N0, size_t... Ns>
  constexpr auto Acc(L l, F&& f, R&& r, std::index_sequence<N0, Ns...>) {
    if constexpr (!m0) return Acc<ms...>(l, std::forward<F>(f), std::forward<R>(r), std::index_sequence<Ns...>{});
    else return Acc<ms...>(l, std::forward<F>(f), f(std::forward<R>(r), l.template Get<N0>()), std::index_sequence<Ns...>{});
  }
  template<class L, class F, class R, size_t N0, size_t... Ns> constexpr auto Acc(L l, F&& f, R&& r, std::index_sequence<N0, Ns...>)
  { return Acc(l, std::forward<F>(f), f(std::forward<R>(r), l.template Get<N0>()), std::index_sequence<Ns...>{}); }
  template<size_t D, class T, class Acc, class F> constexpr auto DFS_Acc(T t, F&& f, Acc&& acc) {
    return t.bases.Accumulate(std::forward<Acc>(acc), [&](auto&& r, auto b) {
      if constexpr (b.is_virtual) return DFS_Acc<D+1>(b.info, std::forward<F>(f), std::forward<decltype(r)>(r));
      else return DFS_Acc<D+1>(b.info, std::forward<F>(f), std::forward<F>(f)(std::forward<decltype(r)>(r), b.info, D+1));
    });
  }
  template<class TI, class U, class F> constexpr void NV_Var(TI info, U&& u, F&& f) {
    info.fields.ForEach([&](auto k){if constexpr(!k.is_static&&!k.is_func)std::forward<F>(f)(k,std::forward<U>(u).*(k.value));});
    info.bases.ForEach([&](auto b){if constexpr(!b.is_virtual)NV_Var(b.info,b.info.Forward(std::forward<U>(u)),std::forward<F>(f));});
  }
}
namespace Ubpa::USRefl {
  template<class Name,class T>struct NamedValue:Name{T value;static constexpr bool has_value=true;constexpr NamedValue(T v):value{v}{}
    template<class U>constexpr bool operator==(U v)const{if constexpr(std::is_same_v<T,U>)return value==v;else return false;} };
  template<class Name> struct NamedValue<Name, void> : Name { /*T value;*/ static constexpr bool has_value = false;
    template<class U> constexpr bool operator==(U) const { return false; } };
  template<typename...Es> struct ElemList {
    std::tuple<Es...> elems; static constexpr size_t size = sizeof...(Es);
    constexpr ElemList(Es... elems) : elems{ elems... } {}
    template<bool... ms, class Init, class Func> constexpr auto Accumulate(Init&& init, Func&& func) const
    { return detail::Acc<ms...>(*this, std::forward<Func>(func), std::forward<Init>(init), std::make_index_sequence<size>{}); }
    template<bool... ms, class Func> constexpr void ForEach(Func&& func) const
    { Accumulate<ms...>(0, [&](auto, auto field) {std::forward<Func>(func)(field); return 0; }); }
    template<class Func> constexpr size_t FindIf(Func&& func) const
    { return detail::FindIf(*this, std::forward<Func>(func), std::make_index_sequence<sizeof...(Es)>{}); }
    template<class S> constexpr auto Find(S = {}) const
    { return Accumulate(0, [](auto r, auto e) {if constexpr (decltype(e)::template NameIs<S>())return e; else return r; }); }
    template<class T> constexpr size_t FindValue(T v) const { return FindIf([v](auto e) { return e == v; }); }
    template<class S>static constexpr bool Contains(S={}) { return (Es::template NameIs<S>() || ...); }
  	template<typename T, typename S>constexpr T ValueOfName(S n) const
  	{return Accumulate(T{},[n](auto r,auto e){if constexpr(std::is_same_v<decltype(e.value), T>)return e.name==n?e.value:r;else return r;});}
    template<class T, class C=char>constexpr auto NameOfValue(T v)const{return Accumulate(std::basic_string_view<C>{},[v](auto r,auto e){return e==v?e.name:r;});}
  	template<class E>constexpr auto Push(E e)const{return std::apply([e](auto...es){return ElemList<Es...,E>{es...,e};},elems);}
    template<class E>constexpr auto Insert(E e)const{if constexpr((std::is_same_v<Es,E>||...))return*this;else return Push(e);}
    template<size_t N> constexpr auto Get() const { return std::get<N>(elems); }
    #define USRefl_ElemList_GetByValue(list, value) list.Get<list.FindValue(value)>()
  };
  template<class Name, class T>struct Attr : NamedValue<Name, T> { constexpr Attr(Name, T v) : NamedValue<Name,T>{ v } {} };
  template<class Name> struct Attr<Name, void> : NamedValue<Name, void> { constexpr Attr(Name) {} };
  template<typename...As>struct AttrList : ElemList<As...> { constexpr AttrList(As...as) : ElemList<As...>{ as... } {} };
  template<bool s, bool f> struct FTraitsB { static constexpr bool is_static = s, is_func = f; };
  template<class T> struct FTraits : FTraitsB<true, false> {}; // default is enum
  template<class U, class T> struct FTraits<T U::*> : FTraitsB<false, std::is_function_v<T>> {};
  template<class T> struct FTraits<T*> : FTraitsB<true, std::is_function_v<T>>{}; // static member
  template<class Name, class T, class AList> struct Field : FTraits<T>, NamedValue<Name, T>
  { AList attrs; constexpr Field(Name, T v, AList as = {}) : NamedValue<Name, T>{ v }, attrs{ as } {} };
  template<typename...Fs>struct FieldList :ElemList<Fs...> { constexpr FieldList(Fs...fs) :ElemList<Fs...>{ fs... } {} };
  template<class T> struct TypeInfo; // TypeInfoBase, name, fields, attrs
  template<class T, bool IsVirtual = false> struct Base
  { static constexpr auto info = TypeInfo<T>{}; static constexpr bool is_virtual = IsVirtual; };
  template<typename...Bs> struct BaseList : ElemList<Bs...> { constexpr BaseList(Bs... bs) : ElemList<Bs...>{ bs... } {} };
  template<typename...Ts> struct TypeInfoList : ElemList<Ts...> { constexpr TypeInfoList(Ts...ts) : ElemList<Ts...>{ ts... } {} };
  template<class T, typename... Bases> struct TypeInfoBase {
    using Type = T; static constexpr BaseList bases{ Bases{}... };
    template<class U> static constexpr auto&& Forward(U&& derived) noexcept {
      if constexpr (std::is_same_v<std::decay_t<U>, U>) return static_cast<Type&&>(derived); // right
      else if constexpr (std::is_same_v<std::decay_t<U>&, U>) return static_cast<Type&>(derived); // left
      else return static_cast<const std::decay_t<U>&>(derived); // std::is_same_v<const std::decay_t<U>&, U>
    }
    static constexpr auto VirtualBases() {
      return bases.Accumulate(ElemList<>{}, [](auto acc, auto base) {
        auto concated = base.info.VirtualBases().Accumulate(acc, [](auto acc, auto b) { return acc.Insert(b); });
        if constexpr (!base.is_virtual) return concated; else return concated.Insert(base.info); });
    }
    template<class R, class F> static constexpr auto DFS_Acc(R&& r, F&& f) {
      return detail::DFS_Acc<0>(TypeInfo<Type>{},std::forward<F>(f),VirtualBases().Accumulate(f(std::forward<R>(r),TypeInfo<Type>{},0),
        [&](auto&& acc, auto vb){ return std::forward<F>(f)(std::forward<decltype(acc)>(acc), vb, 1); })); }
    template<class F>static constexpr void DFS_ForEach(F&&f){DFS_Acc(0,[&](auto,auto t,auto d){std::forward<F>(f)(t,d);return 0;});}
    template<class U, class Func> static constexpr void ForEachVarOf(U&& obj, Func&& func) {
      VirtualBases().ForEach([&](auto vb) { vb.fields.ForEach([&](auto fld)
      { if constexpr (!fld.is_static && !fld.is_func) std::forward<Func>(func)(fld, std::forward<U>(obj).*(fld.value)); }); });
      detail::NV_Var(TypeInfo<Type>{}, std::forward<U>(obj), std::forward<Func>(func)); }
  };
  template<class Name, class Char, size_t N> Attr(Name, const Char(&)[N])->Attr<Name, std::basic_string_view<Char>>;
  template<class Name> Attr(Name)->Attr<Name, void>;
  template<class Name, class T, class AList> Field(Name, T, AList)->Field<Name, T, AList>;
  template<class Name, class T> Field(Name, T)->Field<Name, T, AttrList<>>;
}