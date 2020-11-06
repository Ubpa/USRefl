#include <USRefl/USRefl.h>

#include <iostream>
#include <array>

using namespace Ubpa::USRefl;
using namespace std;

struct [[size(8)]] Point {
	[[not_serialize]]
	float x;
	[[info("hello")]]
	float y;
	float z;
};

template<>
struct Ubpa::USRefl::TypeInfo<Point> :
	TypeInfoBase<Point>
{
#ifdef UBPA_USREFL_NOT_USE_NAMEOF
	static constexpr char name[6] = "Point";
#endif
	static constexpr AttrList attrs = {
		Attr {"size", 8},
	};
	static constexpr FieldList fields = {
		Field {"x", &Type::x, AttrList {
			Attr {"not_serialize"},
		}},
		Field {"y", &Type::y, AttrList {
			Attr {"info", "hello"},
		}},
		Field {"z", &Type::z},
	};
};

template<typename T, size_t... Ns>
constexpr auto GetXZ(std::index_sequence<Ns...>) {
	// get fields with name "x" or "z"
	constexpr auto masks = TypeInfo<T>::fields.Accumulate(
		std::array<bool, TypeInfo<T>::fields.size>{},
		[&, idx = 0](auto acc, auto field) mutable {
			acc[idx] = field.name == "x" || field.name == "z";
			idx++;
			return acc;
		}
	);
	constexpr auto fields = TypeInfo<T>::fields.Accumulate<masks[Ns]...>(
		ElemList<>{},
		[&](auto acc, auto field) {
			return acc.Push(field);
		}
	);
	return fields;
}
template<typename T>
constexpr auto GetXZ() {
	return GetXZ<T>(std::make_index_sequence<TypeInfo<T>::fields.size>{});
}

int main() {
	// get fields with name "x" or "z"
	constexpr auto fields = GetXZ<Point>();
	fields.ForEach([](auto field) {
		cout << field.name << endl;
	});
}
