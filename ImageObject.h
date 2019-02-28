#pragma once

#include <string>
#include <cmath>

class ImageObject {
	public:
		long int xt, yt, area, perimeter;
		double miMax, miMin, F1, F2;
		ImageObject(
				const long int area, const long int perimeter, const long int momentX, const long int momentY, const long int momentX2,
				const long int momentY2, const long int moment11);
};
