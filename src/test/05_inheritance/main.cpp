#include <USRefl/USRefl.h>

#include <iostream>

#include <cassert>

using namespace Ubpa::USRefl;
using namespace std;

struct A { float a; };
struct B : A { float b; };
struct C : A { float c; };
struct D : B, C { float d; };

template<>
struct Ubpa::USRefl::TypeInfo<A> :
	TypeInfoBase<A>
{
	static constexpr AttrList attrs = {};
	static constexpr FieldList fields = {
		Field {TSTR("a"), &Type::a},
	};
};

template<>
struct Ubpa::USRefl::TypeInfo<B> :
	TypeInfoBase<B, Base<A>>
{
	static constexpr AttrList attrs = {};
	static constexpr FieldList fields = {
		Field {TSTR("b"), &Type::b},
	};
};

template<>
struct Ubpa::USRefl::TypeInfo<C> :
	TypeInfoBase<C, Base<A>>
{
	static constexpr AttrList attrs = {};
	static constexpr FieldList fields = {
		Field {TSTR("c"), &Type::c},
	};
};

template<>
struct Ubpa::USRefl::TypeInfo<D> :
	TypeInfoBase<D,
	Base<B>,
	Base<C>
	>
{
	static constexpr AttrList attrs = {};
	static constexpr FieldList fields = {
		Field {TSTR("d"), &Type::d},
	};
};

int main() {
	TypeInfo<D>::DFS_ForEach([](auto t, std::size_t depth) {
		for (std::size_t i = 0; i < depth; i++)
			cout << "  ";
		cout << t.name << endl;
	});

	cout << "[non DFS]" << endl;
	TypeInfo<D>::fields.ForEach([](auto field){
		cout << field.name << endl;
	});

	cout << "[DFS]" << endl;
	TypeInfo<D>::DFS_ForEach([](auto t, std::size_t) {
		t.fields.ForEach([](auto field) {
			cout << field.name << endl;
		});
	});

	constexpr std::size_t fieldNum = TypeInfo<D>::DFS_Acc(0, [](auto acc, auto, auto) {
		return acc + 1;
	});
	cout << "field number : " << fieldNum << endl;

	cout << "[var]" << endl;
	D d;
	d.B::a = 1;
	d.C::a = 2;
	d.b = 3;
	d.c = 4;
	d.d = 5;
	cout << "[left]" << endl;
	TypeInfo<D>::ForEachVarOf(std::move(d), [](auto field, auto&& var) {
		static_assert(std::is_rvalue_reference_v<decltype(var)>);
		cout << field.name << " : " << var << endl;
	});
	cout << "[right]" << endl;
	TypeInfo<D>::ForEachVarOf(d, [](auto field, auto&& var) {
		static_assert(std::is_lvalue_reference_v<decltype(var)>);
		cout << field.name << " : " << var << endl;
	});
}
