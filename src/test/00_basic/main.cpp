#include <USRefl/USRefl.h>
#include <iostream>

using namespace Ubpa::USRefl;
using namespace std;

struct Point {
    float x;
    float y;
};

template<>
struct Ubpa::USRefl::TypeInfo<Point> : TypeInfoBase<Point> {
#ifdef UBPA_USREFL_NOT_USE_NAMEOF
    static constexpr char name[6] = "Point";
#endif
    static constexpr AttrList attrs = {};
    static constexpr FieldList fields = {
      Field {"x", &Type::x},
      Field {"y", &Type::y},
    };
};

int main() {
    Point p{ 1.f, 2.f };
    TypeInfo<Point>::ForEachVarOf(p, [](auto field, auto&& var) {
        cout << field.name << ": " << var << endl;
    });
}
