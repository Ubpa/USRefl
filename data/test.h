struct [[size(8)]] Point {
	[[not_serialize]]
	static const float x = 1.f;
	[[info("hello")]]
	float y{2.f};
	[[attr("great")]]
	float Sum() const{
		return x + y;
	}
	[[attr("great too")]]
	float Sum([[range(std::pair{1.f, 10.f})]]float z = 3.f) const; // overload

	Point& operator+(const Point& rhs) {
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
};
