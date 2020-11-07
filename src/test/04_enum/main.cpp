#include <USRefl/USRefl.h>

#include <iostream>

#include <cassert>

using namespace Ubpa::USRefl;
using namespace std;

template<size_t N>
constexpr size_t Func() { return N; }

enum class [[enum_attr("enum_attr_value")]] Color {
	RED [[enumerator_attr("enumerator_attr_value"), func(&Func<1>)]] = 1,
	GREEN [[func(&Func<2>)]] = 2,
	BLUE [[func(&Func<3>)]] = 4
};

template<>
struct Ubpa::USRefl::TypeInfo<Color> :
    TypeInfoBase<Color>
{
#ifdef UBPA_USREFL_NOT_USE_NAMEOF
    static constexpr char name[6] = "Color";
#endif
    static constexpr AttrList attrs = {
        Attr {USTR("enum_attr"), "enum_attr_value"},
    };
    static constexpr FieldList fields = {
        Field {USTR("RED"), Type::RED, AttrList {
            Attr {USTR("enumerator_attr"), "enumerator_attr_value"},
            Attr {USTR("func"), &Func<1>},
        }},
        Field {USTR("GREEN"), Type::GREEN, AttrList {
            Attr {USTR("func"), &Func<2>},
        }},
        Field {USTR("BLUE"), Type::BLUE, AttrList {
            Attr {USTR("func"), &Func<3>},
        }},
    };
};

int main() {
	cout << TypeInfo<Color>::name << endl;

	TypeInfo<Color>::fields.ForEach([](auto field) {
		cout << field.name << endl;
	});

    Color red = Color::RED;
    std::string_view nameof_red = "RED";

    // name -> value
    {
        // compile-time
        static_assert(TypeInfo<Color>::fields.ValueOfName<Color>("GREEN") == Color::GREEN);

        // runtime
        assert(TypeInfo<Color>::fields.ValueOfName<Color>(nameof_red) == red);
    }

    // value -> name
    {
        // compile-time
        static_assert(TypeInfo<Color>::fields.NameOfValue(Color::GREEN) == "GREEN");

        // runtime
        assert(TypeInfo<Color>::fields.NameOfValue(red) == nameof_red);
    }

    // name -> attr
    {
        // compile-time
        static_assert(TypeInfo<Color>::fields.Find(USTR("GREEN")).attrs.Find(USTR("func")).value() == 2);
        // runtime
        size_t rst = static_cast<size_t>(-1);
        TypeInfo<Color>::fields.FindIf([nameof_red, &rst](auto field) {
            if (field.name == nameof_red) {
                rst = field.attrs.Find(USTR("func")).value();
                return true;
            }
            else
                return false;
        });
        assert(rst == 1);
    }

    // value -> attr
    {
        static_assert(USRefl_ElemList_GetByValue(TypeInfo<Color>::fields, Color::GREEN).attrs.Find(USTR("func")).value() == 2);

        // runtime
        size_t rst = static_cast<size_t>(-1);
        TypeInfo<Color>::fields.FindIf([red, &rst](auto field) {
            if (field.value == red) {
                rst = field.attrs.Find(USTR("func")).value();
                return true;
            }
            else
                return false;
        });
        assert(rst == 1);
    }

}
