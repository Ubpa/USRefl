#include <USRefl/USRefl.h>
#include <iostream>
#include <cmath>
#include <functional>

using namespace Ubpa::USRefl;

struct Vec {
    float x;
    float y;
    float norm() const {
        return std::sqrt(x * x + y * y);
    }
};

template<>
struct Ubpa::USRefl::TypeInfo<Vec> :
    TypeInfoBase<Vec>
{
    static constexpr AttrList attrs = {};
    static constexpr FieldList fields = {
        Field {TSTR("x"), &Type::x},
        Field {TSTR("y"), &Type::y},
        Field {TSTR("norm"), &Type::norm},
    };
};

int main() {
    TypeInfo<Vec>::fields.ForEach([](const auto& field) {
        std::cout << field.name << std::endl;
    });

    Vec v;

    std::invoke(TypeInfo<Vec>::fields.Find(TSTR("x")).value, v) = 3.f;
    std::invoke(TypeInfo<Vec>::fields.Find(TSTR("y")).value, v) = 4.f;
    std::cout << "x: " << std::invoke(TypeInfo<Vec>::fields.Find(TSTR("x")).value, v) << std::endl;

    std::cout << "norm: " << std::invoke(TypeInfo<Vec>::fields.Find(TSTR("norm")).value, v) << std::endl;

    TypeInfo<Vec>::ForEachVarOf(v, [](const auto& field, const auto& var) {
        std::cout << field.name << ": " << var << std::endl;
    });
}
