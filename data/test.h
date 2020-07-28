struct A { float a; };
struct B : virtual A { float b; };
struct C : virtual A { float c; };
struct D : B, C { float d; };