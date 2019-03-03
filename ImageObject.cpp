#include "ImageObject.h"

ImageObject::ImageObject(
		const long int area, const long int perimeter, const long int momentX, const long int momentY) {
	this->perimeter = perimeter;
	this->area = area;
	this->xt = static_cast<double>(momentX) / static_cast<double>(area);
	this->yt = static_cast<double>(momentY) / static_cast<double>(area);
	this->F1 = (perimeter * perimeter) / (100.0 * area);
}

void  ImageObject::setFeatures(const double momentX2, const double momentY2, const double moment11) {
	this->miMin = this->miMax = 0.5 * (momentX2 + momentY2);
	this->miMin -= 0.5 * std::sqrt(4.0 * moment11 * moment11 + (momentX2 - momentY2) * (momentX2 - momentY2));
	this->miMax += 0.5 * std::sqrt(4.0 * moment11 * moment11 + (momentX2 - momentY2) * (momentX2 - momentY2));
	this->F2 = miMin / miMax;
}
