#include "ImageObject.h"

ImageObject::ImageObject(const int area, const int perimeter, const int momentX, const int momentY, const int momentX2,
                         const int momentY2, const int moment11) {
	this->perimeter = perimeter;
	this->area = area;
	this->xt = momentX / area;
	this->yt = momentY / area;
	this->F1 = (perimeter * perimeter) / (100.0 * area);
	this->miMin = this->miMax = 0.5 * (momentX2 + momentY2);
	this->miMin -= 0.5 * std::sqrt(4.0 * moment11 * moment11 + (momentX2 - momentY2) * (momentX2 - momentY2));
	this->miMax += 0.5 * std::sqrt(4.0 * moment11 * moment11 + (momentX2 - momentY2) * (momentX2 - momentY2));
	this->F2 = static_cast<double>(miMin / miMax);

}
