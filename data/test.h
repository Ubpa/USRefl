struct [[size(8)]] Vec2 {
	Vec2() : x{0.f}, y{0.f} {}
	Vec2(float x, float y);
	~Vec2();

	[[not_serialize]]
	static const float x = 1.f;
	[[info("hello")]]
	float y{2.f};
	[[attr("great")]]
	float Sum() const{
		return x + y;
	}
	[[attr("great too")]]
	float Sum([[range(std::pair{1.f, 10.f})]]float z = 3.f) const; overload

	Vec& operator+(const Vec& rhs) {
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
	static float Dot(const Vec2& lhs, const Vec2& rhs){
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}
	static float Dot(const Vec2& v, float k);
	
	template<typename T>
	Point Add(T v){
		return {x + v, y + v};
	}
};
