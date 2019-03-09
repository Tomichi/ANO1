#pragma once
#include <string>
#include <iostream>
#include <cmath>

class ImageObject {
	public:
		long int area, perimeter;
		double miMax, miMin, F1, F2, xt, yt;;
		ImageObject(const long int area, const long int perimeter, const long int momentX, const long int momentY);
		void setFeatures(const double momentX2, const double momentY2, const double moment11);
		void printsImageObject();
};
