#include <USRefl_99.h>
#include <iostream>
#include <array>
#include <cassert>

using namespace Ubpa::USRefl;
using namespace std;

// ==============
//  basic
// ==============
struct [[size(8)]] Point {
	[[not_serialize]]
	float x;
	[[info("hello")]]
	float y;
	static constexpr size_t id = 1024;
	float Sum() const { return x + y; }
};

template<>
struct Ubpa::USRefl::TypeInfo<Point> :
	TypeInfoBase<Point>
{
	static constexpr char name[6] = "Point";
	static constexpr AttrList attrs = {
		Attr {USTR("size"), 8},
	};
	static constexpr FieldList fields = {
		Field {USTR("x"), &Type::x, AttrList {
			Attr {USTR("not_serialize")},
		}},
		Field {USTR("y"), &Type::y, AttrList {
			Attr {USTR("info"), "hello"},
		}},
		Field {USTR("id"), Type::id},
		Field {USTR("Sum"), &Type::Sum},
	};
};

void test_basic() {
	cout
		<< "====================" << endl
		<< " basic" << endl
		<< "====================" << endl;

	Point p{ 1,2 };

	TypeInfo<Point>::fields.ForEach([](auto field) {
		cout << field.name << endl;
		field.attrs.ForEach([](auto attr) {
			cout << attr.name;
			if constexpr (attr.has_value)
				cout << ": " << attr.value;
			cout << endl;
		});
	});

	constexpr auto y_field = TypeInfo<Point>::fields.Find(USTR("y"));
	static_assert(y_field.name == "y");

	static_assert(TypeInfo<Point>::fields.Contains(USTR("x")));

	TypeInfo<Point>::ForEachVarOf(p, [](auto field, auto&& var) {
		cout << field.name << " : " << var << endl;
	});

	TypeInfo<Point>::fields.ForEach([](auto field) {
		if constexpr (field.is_static)
			cout << field.name << ": " << field.value << endl;
	});

	TypeInfo<Point>::fields.ForEach([p](auto field) {
		if constexpr (field.is_func) {
			if (field.name != "Sum")
				return;
			cout << (p.*(field.value))() << endl;
		}
	});
	static_assert(TypeInfo<Point>::fields.NameOfValue(Point::id) == "id");
	static_assert(TypeInfo<Point>::fields.ValueOfName<size_t>("id") == Point::id);
}

// ==============
//  template
// ==============
template<typename T>
struct Data {
	[[range(std::pair<T, T>{static_cast<T>(0), static_cast<T>(100)})]]
	T value;
};

template<typename T>
struct Ubpa::USRefl::TypeInfo<Data<T>> :
	TypeInfoBase<Data<T>>
{
	// [!] all instance types have the same name
	static constexpr char name[5] = "Data";
	static constexpr AttrList attrs = {};
	static constexpr FieldList fields = {
		Field {USTR("value"), &Data<T>::value, AttrList {
			Attr {USTR("range"), std::pair<T,T>{static_cast<T>(0),static_cast<T>(100)}},
		}},
	};
};

void test_template() {
	cout
		<< "====================" << endl
		<< " template" << endl
		<< "====================" << endl;

	cout << TypeInfo<Data<float>>::name << endl;
	constexpr auto valueAttrs = TypeInfo<Data<float>>::fields.Find(USTR("value")).attrs;
	constexpr auto range = valueAttrs.Find(USTR("range")).value;
	constexpr float range_min = range.first;
	constexpr float range_max = range.second;
	cout << "range min :" << range_min << endl;
	cout << "range max :" << range_max << endl;
}

// ==============
//  inheritance
// ==============
struct A { float a; };
struct B : A { float b; };
struct C : A { float c; };
struct D : B, C { float d; };

template<>
struct Ubpa::USRefl::TypeInfo<A> :
	TypeInfoBase<A>
{
	static constexpr char name[2] = "A";
	static constexpr AttrList attrs = {};
	static constexpr FieldList fields = {
		Field {USTR("a"), &Type::a},
	};
};

template<>
struct Ubpa::USRefl::TypeInfo<B> :
	TypeInfoBase<B, Base<A>>
{
	static constexpr char name[2] = "B";
	static constexpr AttrList attrs = {};
	static constexpr FieldList fields = {
		Field {USTR("b"), &Type::b},
	};
};

template<>
struct Ubpa::USRefl::TypeInfo<C> :
	TypeInfoBase<C, Base<A>>
{
	static constexpr char name[2] = "C";
	static constexpr AttrList attrs = {};
	static constexpr FieldList fields = {
		Field {USTR("c"), &Type::c},
	};
};

template<>
struct Ubpa::USRefl::TypeInfo<D> :
	TypeInfoBase<D,
	Base<B>,
	Base<C>
	>
{
	static constexpr char name[2] = "D";
	static constexpr AttrList attrs = {};
	static constexpr FieldList fields = {
		Field {USTR("d"), &Type::d},
	};
};

void test_inheritance() {
	cout
		<< "====================" << endl
		<< " inheritance" << endl
		<< "====================" << endl;

	TypeInfo<D>::DFS_ForEach([](auto t, size_t depth) {
		for (size_t i = 0; i < depth; i++)
			cout << "  ";
		cout << t.name << endl;
	});

	cout << "[non DFS]" << endl;
	TypeInfo<D>::fields.ForEach([](auto field){
		cout << field.name << endl;
	});

	cout << "[DFS]" << endl;
	TypeInfo<D>::DFS_ForEach([](auto t, size_t) {
		t.fields.ForEach([](auto field) {
			cout << field.name << endl;
		});
	});

	cout << "[var]" << endl;
	D d;
	d.B::a = 1;
	d.C::a = 2;
	d.b = 3;
	d.c = 4;
	d.d = 5;
	TypeInfo<D>::ForEachVarOf(std::move(d), [](auto field, auto&& var) {
		static_assert(std::is_rvalue_reference_v<decltype(var)>);
		cout << field.name << " : " << var << endl;
	});
	TypeInfo<D>::ForEachVarOf(d, [](auto field, auto&& var) {
		static_assert(std::is_lvalue_reference_v<decltype(var)>);
		cout << field.name << " : " << var << endl;
	});
}

// ==============
//  enum
// ==============
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
	static constexpr char name[6] = "Color";
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

void test_enum() {
	cout
		<< "====================" << endl
		<< " enum" << endl
		<< "====================" << endl;
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

// ==============
//  function
// ==============
struct FuncList {
	void Func0(float a, float b) {}
	void Func1(int x = 1) {}
};

template<>
struct Ubpa::USRefl::TypeInfo<FuncList> :
	TypeInfoBase<FuncList>
{
	static constexpr char name[9] = "FuncList";
	static constexpr AttrList attrs = {};
	static constexpr FieldList fields = {
		Field {USTR("Func0"), &Type::Func0},
		Field {USTR("Func1"), &Type::Func1, AttrList {
			Attr {USTR("default_functions"), std::tuple {
				[](Type* __this) { return __this->Func1(); }
			}},
		}},
	};
};

void test_function() {
	cout
		<< "====================" << endl
		<< " function" << endl
		<< "====================" << endl;

	TypeInfo<FuncList>::fields.ForEach([](auto field) {
		cout << field.name << endl;
	});
}

// ==============
//  virtual
// ==============
struct VA { float a; };
struct VB : virtual VA { float b; };
struct VC : virtual VA { float c; };
struct VD : VB, VC { float d; };

template<>
struct Ubpa::USRefl::TypeInfo<VA> :
    TypeInfoBase<VA>
{
    static constexpr char name[3] = "VA";
    static constexpr AttrList attrs = {};
    static constexpr FieldList fields = {
        Field {USTR("a"), &Type::a},
    };
};

template<>
struct Ubpa::USRefl::TypeInfo<VB> :
    TypeInfoBase<VB, Base<VA, true>>
{
    static constexpr char name[3] = "VB";
    static constexpr AttrList attrs = {};
    static constexpr FieldList fields = {
        Field {USTR("b"), &Type::b},
    };
};

template<>
struct Ubpa::USRefl::TypeInfo<VC> :
    TypeInfoBase<VC, Base<VA, true>>
{
    static constexpr char name[3] = "VC";
    static constexpr AttrList attrs = {};
    static constexpr FieldList fields = {
        Field {USTR("c"), &Type::c},
    };
};

template<>
struct Ubpa::USRefl::TypeInfo<VD> :
    TypeInfoBase<VD,
        Base<VB>,
        Base<VC>
    >
{
    static constexpr char name[3] = "VD";
    static constexpr AttrList attrs = {};
    static constexpr FieldList fields = {
        Field {USTR("d"), &Type::d},
    };
};

void test_virtual() {
	cout
		<< "====================" << endl
		<< " virtual" << endl
		<< "====================" << endl;

	cout << "// not fully support in MSVC++ 19.26 because of a bug (2020/07/17)" << endl;
	cout << "// https://developercommunity.visualstudio.com/content/problem/1116835/member-pointer-of-a-class-with-a-virtual-base-1.html" << endl;

	cout << "[Virtual Bases]" << endl;
	constexpr auto vbs = TypeInfo<D>::VirtualBases();
	vbs.ForEach([](auto info) {
		cout << info.name << endl;
	});

	cout << "[Tree]" << endl;
	TypeInfo<VD>::DFS_ForEach([](auto t, size_t depth) {
		for (size_t i = 0; i < depth; i++)
			cout << "  ";
		cout << t.name << endl;
	});

	cout << "[field]" << endl;
	TypeInfo<VD>::DFS_ForEach([](auto t, size_t) {
		t.fields.ForEach([](auto field) {
			cout << field.name << endl;
			});
	});

	cout << "[var]" << endl;
	VD d;
	d.a = 1;
	d.b = 2;
	d.c = 3;
	d.d = 4;
	cout << "[var : left]" << endl;
	TypeInfo<VD>::ForEachVarOf(std::move(d), [](auto field, auto&& var) {
		static_assert(std::is_rvalue_reference_v<decltype(var)>);
		cout << field.name << " : " << var << endl;
	});
	cout << "[var : right]" << endl;
	TypeInfo<VD>::ForEachVarOf(d, [](auto field, auto&& var) {
		static_assert(std::is_lvalue_reference_v<decltype(var)>);
		cout << field.name << " : " << var << endl;
	});
}

// ==============
//  mask
// ==============
template<typename T, size_t... Ns>
constexpr auto GetXID(std::index_sequence<Ns...>) {
	// get fields with name "x" or "id"
	constexpr auto masks = TypeInfo<T>::fields.Accumulate(
		std::array<bool, TypeInfo<T>::fields.size>{},
		[&, idx = 0](auto acc, auto field) mutable {
		acc[idx] = field.name == "x" || field.name == "id";
		idx++;
		return acc;
	}
	);
	constexpr auto fields = TypeInfo<T>::fields.template Accumulate<masks[Ns]...>(
		ElemList<>{},
		[&](auto acc, auto field) {
			return acc.Push(field);
		}
	);
	return fields;
}
template<typename T>
constexpr auto GetXID() {
	return GetXID<T>(std::make_index_sequence<TypeInfo<T>::fields.size>{});
}

void test_mask() {
	cout
		<< "====================" << endl
		<< " mask" << endl
		<< "====================" << endl;

	// get fields with name "x" or "id"
	constexpr auto fields = GetXID<Point>();
	fields.ForEach([](auto field) {
		cout << field.name << endl;
	});
}

int main() {
	test_basic();
	test_template();
	test_enum();
	test_inheritance();
	test_function();
	test_virtual();
	test_mask();

	return 0;
}
