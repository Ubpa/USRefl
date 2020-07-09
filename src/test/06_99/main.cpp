#include "USRefl_99.h"
#include <iostream>
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
struct Type<Point> {
	static constexpr std::string_view name = "Point";
	using type = Point;

	static constexpr FieldList fields = {
		Field{"x", &Point::x, AttrList{ Attr{ "not_serialize" } }},
		Field{"y", &Point::y, AttrList{ Attr{ "info", "hello" } }},
		Field{"id", &Point::id, AttrList{ }},
		Field{"Sum", &Point::Sum, AttrList{ }}
	};

	static constexpr AttrList attrs = {
		Attr{ "size", 8 }
	};
};

void test_basic() {
	cout
		<< "====================" << endl
		<< " basic" << endl
		<< "====================" << endl;

	Point p{ 1,2 };

	Type<Point>::fields.ForEach([](auto field) {
		cout << field.name << endl;
		field.attrs.ForEach([](auto attr) {
			cout << attr.name << endl;
			if constexpr (attr.has_value)
				cout << ": " << attr.value << endl;
		});
	});

	constexpr auto y_idx = Type<Point>::fields.Find("y");
	constexpr auto y_field = Type<Point>::fields.Get<y_idx>();
	static_assert(y_field.name == "y");

	static_assert(Type<Point>::fields.Contains("x"));

	ForEachVarOf(p, [](auto&& var) {
		cout << var << endl;
	});

	Type<Point>::fields.ForEach([](auto field) {
		if constexpr (field.is_static)
			cout << field.name << ": " << *field.value << endl;
	});

	Type<Point>::fields.ForEach([p](auto field) {
		if constexpr (field.is_function) {
			if (field.name != "Sum")
				return;
			cout << (p.*(field.value))() << endl;
		}
	});
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
struct Type<Data<T>> {
	static constexpr std::string_view name = "Data"; // use nameof
	using type = Data<T>;

	static constexpr FieldList fields = {
		Field{"value", &Data<T>::value, AttrList{ Attr{"range", std::pair<T, T>{static_cast<T>(0), static_cast<T>(100)}} }}
	};
};

void test_template() {
	cout
		<< "====================" << endl
		<< " template" << endl
		<< "====================" << endl;

	cout << Type<Data<float>>::name << endl;
	constexpr auto valueIdx = Type<Data<float>>::fields.Find("value");
	constexpr auto valueAttrs = Type<Data<float>>::fields.Get<valueIdx>().attrs;
	constexpr auto rangeIdx = valueAttrs.Find("range");
	constexpr auto range = valueAttrs.Get<rangeIdx>().value;
	constexpr float range_min = range.first;
	constexpr float range_max = range.second;
	cout << "range min :" << range_min << endl;
	cout << "range max :" << range_max << endl;
}

// ==============
//  inheritance
// ==============
struct A { float a; };
struct B { float b; };
struct C : A { float c; };
struct D : B, C { float d; };

template<>
struct Type<A> {
	static constexpr std::string_view name = "A";
	using type = A;
	static constexpr TypeList bases = {};

	static constexpr FieldList fields = FieldList{
		Field{"a", &A::a, AttrList{} }
	}.UnionTypeList(bases);

	static constexpr AttrList attrs = {};
};

template<>
struct Type<B> {
	static constexpr std::string_view name = "B";
	using type = B;
	static constexpr TypeList bases = {};

	static constexpr FieldList fields = FieldList{
		Field{"b", &B::b, AttrList{} }
	}.UnionTypeList(bases);

	static constexpr AttrList attrs = {};
};

template<>
struct Type<C> {
	static constexpr std::string_view name = "C";
	using type = C;
	static constexpr TypeList bases = { Type<A>{} };

	static constexpr FieldList fields = FieldList{
		Field{"c", &C::c, AttrList{} }
	}.UnionTypeList(bases);

	static constexpr AttrList attrs = {};
};

template<>
struct Type<D> {
	static constexpr std::string_view name = "D";
	using type = D;
	static constexpr TypeList bases = { Type<B>{}, Type<C>{} };

	static constexpr FieldList fields = FieldList{
		Field{"d", &D::d, AttrList{} }
	}.UnionTypeList(bases);

	static constexpr AttrList attrs = {};
};

template<typename T>
void dump(size_t depth = 0) {
	for (size_t i = 0; i < depth; i++)
		cout << "  ";
	cout << Type<T>::name << endl;
	Type<T>::bases.ForEach([depth](auto t) {
		dump<typename decltype(t)::type>(depth + 1);
		});
}

void test_inheritance() {
	cout
		<< "====================" << endl
		<< " inheritance" << endl
		<< "====================" << endl;

	dump<D>();

	Type<D>::fields.ForEach([](auto field){
		cout << field.name << endl;
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
struct Type<Color> {
	static constexpr std::string_view name = "Color";
	using type = Color;

	static constexpr FieldList fields = {
		Field{"RED", Color::RED, AttrList{ Attr{ "enumerator_attr", "enumerator_attr_value" },Attr{"func", &Func<1>} }},
		Field{"GREEN", Color::GREEN, AttrList{ Attr{"func", &Func<2>} }},
		Field{"BLUE", Color::BLUE, AttrList{ Attr{"func", &Func<3>} }}
	};

	static constexpr AttrList attrs = {
		Attr{ "enum_attr", "enum_attr_value" }
	};
};

void test_enum() {
	cout
		<< "====================" << endl
		<< " enum" << endl
		<< "====================" << endl;

	Type<Color>::fields.ForEach([](auto field) {
		cout << field.name << endl;
	});
	static_assert(Type<Color>::fields.Get<Type<Color>::fields.Find("RED")>().value == Color::RED);
	static_assert(Type<Color>::fields.Get<Type<Color>::fields.FindByValue(Color::RED)>().name == "RED");

	constexpr Color c = Color::GREEN;
	constexpr auto c_attr = Type<Color>::fields.Get<Type<Color>::fields.FindByValue(c)>().attrs;
	static_assert(c_attr.Get<c_attr.Find("func")>().value() == 2);
}

int main() {
	test_basic();
	test_template();
	test_enum();
	test_inheritance();

	return 0;
}
