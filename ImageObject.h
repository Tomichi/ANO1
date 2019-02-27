#pragma once

#include <string>
#include <cmath>

class ImageObject {
	public:
		int xt, yt, area, perimeter;
		double miMax, miMin, F1, F2;
		ImageObject(
				const int area, const int perimeter, const int momentX, const int momentY, const int momentX2,
				const int momentY2, const int moment11);
};
