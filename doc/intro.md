# Introduction

We have provided several examples to demonstrate the capabilities of the library.

> You can find them in the [README](../README.md) 

All you need to do is writing a declaration for the class which needs to be reflected.

The declaration is simple enough that we don't need any marco to simplify it. **We also provide a parser to auto generate the declaration file**.

The library is well-designed so it's very tiny (only 99 lines), so it's easy to get the whole idea of this library. [USRefl_99.h](../include/USRefl_99.h) is condensed from  [USRefl.h](../include/USRefl/USRefl.h) which is much easy to read. Both of them has the same API.

> the difference between USRefl_99.h and USRefl.h
>
> - `name` 

## 1. Design

The library is noninvasive and static.

We have provided several template classes to support the reflection, including

- `template<typename T> NamedValue`: a **value** with a **name** (if value's type `T` is `void`, it means this object has no value but a name).
- `template<typename... Elems> ElemList`: element list, the elements's types `Elems...` can be attributes, fields, type informations or bases. It is a base class of their list. Acctually, it's a `std::tuple`.
- `template<typename T> Attr`: a field may have some attributes (key + value), value's type is `T`，it can be `void`.
- `template<typename T, typename AList> Field`: a field is a named value with a attribute list. The value's type is `T`, it can be (non-)static member variable/function pointer or a key of an enum.
- `template<typename T> TypeInfo`: this template class is the only part you need to write. You should partial specialization it and write some static informations of the type `T` you need to reflect. You can read the example codes to see how it is declared easily.
- `template<typename T, bool IsVirtual=false> Base`: a template class to tag a base type `T` of the type which you're declaring. the template arguments `bool IsVirtual` is for the virtual inheritance.
- list: there are some template list class which are derived from `ElemList`, has several functions for searching, accumulating and so on.
  - `template<typename... Attrs> AttrList`: attribute list, `Attrs...` are attribute types `Attr`.
  - `template<typename... Fields> FieldList`: field list, `Fields...` are field types `Field`.
  - `template<typename... Bases> BaseList`: base list, `Bases` are base tag types `Base`.
- `template<typename T, typename... Bases> TypeInfoBase`: a helper template class for the declaration of type `T`. It provides some functions and a base list for the type `T`.

Here is a brief summary

- `Attr`: name [+ value]
- `Field`: name + value + attribute list
- `TypeInfo`: TypeInfoBase ( bases ) + name + field list + attribute list
- `Base`: type + `IsVirtual` 
- list: `ElemList` -> `AttrList`, `FieldList`, `BaseList` 

## 2. Example

Suppose that the type `Point` is need to be reflected.

```c++
struct [[size(8)]] Point {
  [[not_serialize]]
  float x;
  [[info("hello")]]
  float y;
};
```

`[[...]]` are attributes, but they are useless in the code, so they act like comments.

Then we need to write a declaration for it.

```c++
template<>
struct TypeInfo<Point> : TypeInfoBase<Point> {
  // the name is declared by TypeInfoBase<Point>
  // if you use USRefl_99.h, you should declare it here.
  // static constexpr std::string_view name = "struct Point";

  static constexpr FieldList fields = {
    Field{"x", &Point::x, AttrList{ Attr{ "not_serialize" } }},
    Field{"y", &Point::y, AttrList{ Attr{ "info", "hello" } }}
  };

  static constexpr AttrList attrs = {
    Attr{ "size", 8 }
  };
};
```

The important points are

- `TypeInfoBase<Point>` 
- `name` 
- `fields` 
- `attrs` 

It's easy to write if you are familiar how it is declared.

Then you can write some code with the relection.

**iterate fields and attrs of fields** 

```c++
TypeInfo<Point>::fields.ForEach([](auto field) {
  cout << field.name << endl;
  field.attrs.ForEach([](auto attr) {
    cout << attr.name;
    if constexpr (attr.has_value)
      cout << ": " << attr.value;
    cout << endl;
  });
});
```

**query** 

```c++
static_assert(TypeInfo<Point>::fields.Contains("x"));
```

**iterate attrs of the type** 

```c++
TypeInfo<Point>::attrs.ForEach([](auto attr) {
  cout << attr.name;
  if constexpr (!attr.has_value)
    cout << ": " << attr.value;
  cout << endl;
});
```

**iterate member variables of an object** 

```c++
TypeInfo<Point>::ForEachVarOf(Point{ 1,2 }, [](auto&& var) {
  cout << var << endl;
});
```

