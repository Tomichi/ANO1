#pragma once

#include <string>

class ImageObject {
	private:
		int xt, yt, area, perimeter;
	public:
		ImageObject(int area, int perimeter, long int momentX, long int momentY);
		const int getArea() const;
		const int getPerimeter() const;
		const int getXt() const;
		const int getYt() const;
};
