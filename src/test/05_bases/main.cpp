#include <USRefl.h>

#include <iostream>

#include <cassert>

using namespace Ubpa::USRefl;
using namespace std;

struct A {
	float a;
};
struct B : A {
	float b;
};
struct C : A {
	float c;
};
struct D : B, C {
	float d;
};

template<>
struct TypeInfo<A> : TypeInfoBase<TypeInfo<A>> {
	static constexpr std::string_view name = "A";

	static constexpr FieldList fields = FieldList{
		Field{"a", &A::a, AttrList{} }
	};

	static constexpr AttrList attrs = {};
};

template<>
struct TypeInfo<B> : TypeInfoBase<TypeInfo<B>, A> {
	static constexpr std::string_view name = "B";

	static constexpr FieldList fields = FieldList{
		Field{"b", &B::b, AttrList{} }
	};

	static constexpr AttrList attrs = {};
};

template<>
struct TypeInfo<C> : TypeInfoBase<TypeInfo<C>, A> {
	static constexpr std::string_view name = "C";

	static constexpr FieldList fields = FieldList{
		Field{"c", &C::c, AttrList{} }
	};

	static constexpr AttrList attrs = {};
};

template<>
struct TypeInfo<D> : TypeInfoBase<TypeInfo<D>, B, C> {
	static constexpr std::string_view name = "D";

	static constexpr FieldList fields = FieldList{
		Field{"d", &D::d, AttrList{} }
	};

	static constexpr AttrList attrs = {};
};

template<typename T>
void dump(size_t depth = 0) {
	for (size_t i = 0; i < depth; i++)
		cout << "  ";
	cout << TypeInfo<T>::name << endl;
	TypeInfo<T>::bases.ForEach([depth](auto t) {
		dump<typename decltype(t)::type>(depth + 1);
	});
}

int main() {
	dump<D>();

	TypeInfo<D>::fields.ForEach([](auto field){
		cout << field.name << endl;
	});
	TypeInfo<D>::DFS([](auto t) {
		t.fields.ForEach([](auto field) {
			cout << field.name << endl;
		});
	});

	D d;
	d.B::a = 1;
	d.C::a = 2;
	d.b = 3;
	d.c = 4;
	d.d = 5;
	TypeInfo<D>::DFS_ForEachVarOf(std::move(d), [](auto&& var) {
		static_assert(std::is_rvalue_reference_v<decltype(var)>);
		cout << var << endl;
	});
}
