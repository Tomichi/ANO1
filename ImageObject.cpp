#include "ImageObject.h"

ImageObject::ImageObject(
		const long int area, const long int perimeter, const long int momentX, const long int momentY) {
	this->perimeter = perimeter;
	this->area = area;
	this->xt = momentX / area;
	this->yt = momentY / area;
	this->F1 = (perimeter * perimeter) / (100.0 * area);
}

void  ImageObject::setFeatures(const long int momentX2, const long int momentY2, const long int moment11) {
	this->miMin = this->miMax = 0.5 * (momentX2 + momentY2);
	this->miMin -= 0.5 * std::sqrt(4.0 * moment11 * moment11 + (momentX2 - momentY2) * (momentX2 - momentY2));
	this->miMax += 0.5 * std::sqrt(4.0 * moment11 * moment11 + (momentX2 - momentY2) * (momentX2 - momentY2));
	this->F2 = miMin / miMax;
}
