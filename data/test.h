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