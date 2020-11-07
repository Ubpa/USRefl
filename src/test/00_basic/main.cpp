#include <USRefl/USRefl.h>
#include <iostream>

using namespace Ubpa::USRefl;
using namespace std;

struct Point {
    float x;
    float y;
};

template<>
struct Ubpa::USRefl::TypeInfo<Point> :
    TypeInfoBase<Point>
{
#ifdef UBPA_USREFL_NOT_USE_NAMEOF
    static constexpr char name[6] = "Point";
#endif
    static constexpr AttrList attrs = {};
    static constexpr FieldList fields = {
        Field {USTR("x"), &Type::x},
        Field {USTR("y"), &Type::y},
    };
};

int main() {
    Point p{ 1.f, 2.f };
    TypeInfo<Point>::ForEachVarOf(p, [](auto field, auto&& var) {
        cout << field.name << ": " << var << endl;
    });
    constexpr auto field_x = TypeInfo<Point>::fields.Find(USTR("x"));
    cout << p.*field_x.value << endl;
    constexpr bool contains_y = TypeInfo<Point>::fields.Contains(USTR("y"));
    static_assert(contains_y);
}
