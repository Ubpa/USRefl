# USRefl
**U**bpa **S**tatic **R**eflection

[![repo-size](https://img.shields.io/github/languages/code-size/Ubpa/USRefl?style=flat)](https://github.com/Ubpa/USRefl/archive/master.zip) [![tag](https://img.shields.io/github/v/tag/Ubpa/USRefl)](https://github.com/Ubpa/USRefl/tags) [![license](https://img.shields.io/github/license/Ubpa/USRefl)](LICENSE) 

## Feature

- the **best** C++ static reflection libliray in the whole world
- tiny, handy, elegant
- noninvasive
- basic
  - (non-static / static) member variable
  - (non-static / static) member function
- attribute
- enum
  - string <-> key
  - static dispatch
- template
- inheritance
  - diamond inheritance
  - inherit field (member variable, member function)
  - iterate bases recursively

## Example

```c++
#include <USRefl.h>
#include <iostream>
using namespace Ubpa::USRefl;
using namespace std;

// [[...]] act as (structured) command
// they are useless in code
struct [[size(8)]] Point {
  [[not_serialize]]
  float x;
  [[info("hello")]]
  float y;
};

template<>
struct TypeInfo<Point> : TypeInfoBase<Point> {
  static constexpr std::string_view name = "Point";

  static constexpr FieldList fields = {
    Field{"x", &Point::x, AttrList{ Attr{ "not_serialize" } }},
    Field{"y", &Point::y, AttrList{ Attr{ "info", "hello" } }}
  };

  static constexpr AttrList attrs = {
    Attr{ "size", 8 }
  };
};

int main() {
  TypeInfo<Point>::fields.ForEach([](auto field) {
    cout << field.name << endl;
    field.attrs.ForEach([](auto attr) {
      cout << attr.name << endl;
      if constexpr (attr.has_value)
        cout << ": " << attr.value;
      cout << endl;
    });
  });

  static_assert(Type<Point>::fields.Contains("x"));

  TypeInfo<Point>::attrs.ForEach([](auto attr) {
    cout << attr.name;
    if constexpr (!attr.has_value)
      cout << ": " << attr.value;
    cout << endl;
  });
  
  TypeInfo<Point>::ForEachVarOf(Point{ 1,2 }, [](auto&& var) {
    cout << var << endl;
  });
}
```

**other example** 

- [template](src/test/01_template/main.cpp) 
- [static](src/test/02_static/main.cpp) 
- [func](src/test/03_func/main.cpp) 
- [enum](src/test/04_enum/main.cpp) 
- [inheritance](src/test/05_subclass/main.cpp) 
- 99 line: [USRefl_99.h](src/test/06_99/USRefl_99.h)，test [USRefl_99.h](src/test/06_99/main.h) 

## Integration

You should add required file [USRefl.h](include/USRefl.h) 

