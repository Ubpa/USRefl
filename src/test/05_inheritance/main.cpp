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
#ifdef UBPA_USREFL_NOT_USE_NAMEOF
	static constexpr char name[2] = "A";
#endif
	static constexpr AttrList attrs = {};
	static constexpr FieldList fields = {
		Field {USTR("a"), &Type::a},
	};
};

template<>
struct Ubpa::USRefl::TypeInfo<B> :
	TypeInfoBase<B, Base<A>>
{
#ifdef UBPA_USREFL_NOT_USE_NAMEOF
	static constexpr char name[2] = "B";
#endif
	static constexpr AttrList attrs = {};
	static constexpr FieldList fields = {
		Field {USTR("b"), &Type::b},
	};
};

template<>
struct Ubpa::USRefl::TypeInfo<C> :
	TypeInfoBase<C, Base<A>>
{
#ifdef UBPA_USREFL_NOT_USE_NAMEOF
	static constexpr char name[2] = "C";
#endif
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
#ifdef UBPA_USREFL_NOT_USE_NAMEOF
	static constexpr char name[2] = "D";
#endif
	static constexpr AttrList attrs = {};
	static constexpr FieldList fields = {
		Field {USTR("d"), &Type::d},
	};
};

int main() {
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

	constexpr size_t fieldNum = TypeInfo<D>::DFS_Acc(0, [](auto acc, auto, auto) {
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
