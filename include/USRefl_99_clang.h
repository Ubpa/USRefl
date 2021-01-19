#pragma once                           // Ubpa Static Reflection -- 99 lines
#include <string_view>                 // Repository: https://github.com/Ubpa/USRefl
#include <tuple>                       // License: https://github.com/Ubpa/USRefl/blob/master/LICENSE
#define TSTR(s) ([] { struct tmp { static constexpr auto get() { return std::basic_string_view{s}; } }; \
  return Ubpa::detail::TSTRH(tmp{}); }())
namespace Ubpa::detail {
  template<typename C, C... chars> struct TStr { using Char = C;
    template<typename T> static constexpr bool Is(T = {}) { return std::is_same_v<T, TStr>; }
    static constexpr const Char* Data() { return data; } static constexpr std::size_t Size() { return sizeof...(chars); }
    static constexpr std::basic_string_view<Char> View() { return data; }
  private: static constexpr Char data[]{ chars...,Char(0) }; };
  template<typename Char, typename T, std::size_t... Ns> constexpr auto TSTRHI(std::index_sequence<Ns...>) { return TStr<Char, T::get()[Ns]...>{}; }
  template<typename T> constexpr auto TSTRH(T){return TSTRHI<typename decltype(T::get())::value_type,T>(std::make_index_sequence<T::get().size()>{});}
  template<class L, class F> constexpr std::size_t FindIf(const L&, F&&, std::index_sequence<>) { return -1; }
  template<class L, class F, std::size_t N0, std::size_t... Ns> constexpr std::size_t FindIf(const L& l, F&& f, std::index_sequence<N0, Ns...>)
  { return f(l.template Get<N0>()) ? N0 : FindIf(l, std::forward<F>(f), std::index_sequence<Ns...>{}); }
  template<class L, class F, class R> constexpr auto Acc(const L&, F&&, R r, std::index_sequence<>) { return r; }
  template<class L, class F, class R, std::size_t N0, std::size_t... Ns> constexpr auto Acc(const L& l, F&& f, R r, std::index_sequence<N0, Ns...>)
  { return Acc(l, std::forward<F>(f), f(std::move(r), l.template Get<N0>()), std::index_sequence<Ns...>{}); }
  template<std::size_t D, class T, class R, class F> constexpr auto DFS_Acc(T t, F&& f, R r) {
    return t.bases.Accumulate(std::move(r), [&](auto r, auto b) {
      if constexpr (b.is_virtual) return DFS_Acc<D+1>(b.info, std::forward<F>(f), std::move(r));
      else return DFS_Acc<D+1>(b.info, std::forward<F>(f), std::forward<F>(f)(std::move(r), b.info, D+1)); });
  }
  template<class TI, class U, class F> constexpr void NV_Var(TI, U&& u, F&& f) {
    TI::fields.ForEach([&](auto&& k){using K=std::decay_t<decltype(k)>;
      if constexpr(!K::is_static&&!K::is_func)std::forward<F>(f)(k,std::forward<U>(u).*(k.value));});
    TI::bases.ForEach([&](auto b){if constexpr(!b.is_virtual)NV_Var(b.info,b.info.Forward(std::forward<U>(u)),std::forward<F>(f));});
  }
}
namespace Ubpa::USRefl {
  template<class Name>struct NamedValueBase { using TName = Name; static constexpr std::string_view name = TName::View(); };
  template<class Name, class T>struct NamedValue : NamedValueBase<Name> { T value; static constexpr bool has_value = true;
    constexpr NamedValue(T v) : value{ v } {}
    template<class U> constexpr bool operator==(U v) const { if constexpr (std::is_same_v<T, U>) return value == v; else return false; } };
  template<class Name> struct NamedValue<Name, void> : NamedValueBase<Name> { /*T value;*/ static constexpr bool has_value = false;
    template<class U> constexpr bool operator==(U) const { return false; } };
  template<typename...Es> struct ElemList {
    std::tuple<Es...> elems; static constexpr std::size_t size = sizeof...(Es);
    constexpr ElemList(Es... elems) : elems{ elems... } {}
    template<class Init, class Func> constexpr auto Accumulate(Init init, Func&& func) const
    { return detail::Acc(*this, std::forward<Func>(func), std::move(init), std::make_index_sequence<size>{}); }
    template<class F> constexpr void ForEach(F&& f) const { Accumulate(0, [&](auto, const auto& field) {std::forward<F>(f)(field); return 0; }); }
    template<class S> static constexpr bool Contains(S = {}) { return (Es::TName::template Is<S>() || ...); }
    template<class F> constexpr std::size_t FindIf(F&& f)const { return detail::FindIf(*this, std::forward<F>(f), std::make_index_sequence<size>{}); }
    template<class S> constexpr const auto& Find(S = {}) const { constexpr std::size_t idx = []() { constexpr decltype(S::View()) names[]{ Es::name... };
      for (std::size_t i=0; i < size; i++) { if (S::View() == names[i]) return i; } return static_cast<std::size_t>(-1); }(); return Get<idx>(); }
    template<class T> constexpr std::size_t FindValue(const T& v) const { return FindIf([&v](auto e) { return e == v; }); }
    template<typename T, typename S> constexpr const T* ValuePtrOfName(S n) const
    { return Accumulate(nullptr, [n](auto r, const auto& e) {
        if constexpr (std::is_same_v<decltype(e.value), T>) return e.name == n ? &e.value : r; else return r; }); }
    template<typename T, typename S>constexpr const T& ValueOfName(S n) const { return *ValuePtrOfName<T>(n); }
    template<class T, class C = char> constexpr auto NameOfValue(const T& v) const
    { return Accumulate(std::basic_string_view<C>{}, [&v](auto r, auto&& e) {return e == v ? e.name : r; }); }
  	template<class E> constexpr auto Push(E e)const{return std::apply([e](auto...es){return ElemList<Es...,E>{es...,e};},elems);}
    template<class E> constexpr auto Insert(E e)const{if constexpr((std::is_same_v<Es,E>||...))return*this;else return Push(e);}
    template<std::size_t N> constexpr const auto& Get() const { return std::get<N>(elems); }
    #define USRefl_ElemList_GetByValue(list, value) list.Get<list.FindValue(value)>()
  };
  template<class Name, class T>struct Attr : NamedValue<Name, T> { constexpr Attr(Name, T v) : NamedValue<Name,T>{ v } {} };
  template<class Name> struct Attr<Name, void> : NamedValue<Name, void> { constexpr Attr(Name) {} };
  template<typename...As> struct AttrList : ElemList<As...> { constexpr AttrList(As...as) : ElemList<As...>{ as... } {} };
  template<bool s, bool f> struct FTraitsB { static constexpr bool is_static = s, is_func = f; };
  template<class T> struct FTraits : FTraitsB<true, false> {}; // default is enum
  template<class U, class T> struct FTraits<T U::*> : FTraitsB<false, std::is_function_v<T>> {};
  template<class T> struct FTraits<T*> : FTraitsB<true, std::is_function_v<T>>{}; // static member
  template<class Name, class T, class AList> struct Field : FTraits<T>, NamedValue<Name, T>
  { AList attrs; constexpr Field(Name, T v, AList as = {}) : NamedValue<Name, T>{ v }, attrs{ as } {} };
  template<typename...Fs> struct FieldList :ElemList<Fs...> { constexpr FieldList(Fs...fs) :ElemList<Fs...>{ fs... } {} };
  template<class T> struct TypeInfo; // TypeInfoBase, name, fields, attrs
  template<class T, bool IsVirtual = false> struct Base
  { static constexpr auto info = TypeInfo<T>{}; static constexpr bool is_virtual = IsVirtual; };
  template<typename...Bs> struct BaseList : ElemList<Bs...> { constexpr BaseList(Bs... bs) : ElemList<Bs...>{ bs... } {} };
  template<typename...Ts> struct TypeInfoList : ElemList<Ts...> { constexpr TypeInfoList(Ts...ts) : ElemList<Ts...>{ ts... } {} };
  template<class T, typename... Bases> struct TypeInfoBase {
    using Type = T; static constexpr BaseList bases{ Bases{}... };
    template<class U> static constexpr auto&& Forward(U&& derived) {
      if constexpr (std::is_same_v<std::decay_t<U>, U>) return static_cast<Type&&>(derived); // right
      else if constexpr (std::is_same_v<std::decay_t<U>&, U>) return static_cast<Type&>(derived); // left
      else return static_cast<const std::decay_t<U>&>(derived); // std::is_same_v<const std::decay_t<U>&, U>
    }
    static constexpr auto VirtualBases() {
      return bases.Accumulate(ElemList<>{}, [](auto acc, auto base) {
        auto concated = base.info.VirtualBases().Accumulate(acc, [](auto acc, auto b) { return acc.Insert(b); });
        if constexpr (!base.is_virtual) return concated; else return concated.Insert(base.info); });
    }
    template<class R, class F> static constexpr auto DFS_Acc(R r, F&& f) {
      return detail::DFS_Acc<0>(TypeInfo<Type>{}, std::forward<F>(f),
        VirtualBases().Accumulate(std::forward<F>(f)(std::move(r), TypeInfo<Type>{}, 0),
          [&](auto acc, auto vb) { return std::forward<F>(f)(std::move(acc), vb, 1); })); }
    template<class F> static constexpr void DFS_ForEach(F&& f) { DFS_Acc(0, [&](auto, auto t, auto d) {std::forward<F>(f)(t, d); return 0; }); }
    template<class U, class Func> static constexpr void ForEachVarOf(U&& obj, Func&& func) {
      VirtualBases().ForEach([&](auto vb) { vb.fields.ForEach([&](const auto& fld){ using Fld = std::decay_t<decltype(fld)>;
      if constexpr (!Fld::is_static && !Fld::is_func) std::forward<Func>(func)(fld, std::forward<U>(obj).*(fld.value)); }); });
      detail::NV_Var(TypeInfo<Type>{}, std::forward<U>(obj), std::forward<Func>(func)); }
  };
  template<class Name> Attr(Name)->Attr<Name, void>;
  template<class Name, class T> Field(Name, T)->Field<Name, T, AttrList<>>;
}