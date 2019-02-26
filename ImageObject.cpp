#include "ImageObject.h"

ImageObject::ImageObject(int area, int perimeter, int momentX, int momentY) {
	this->area = area;
	this->perimeter = perimeter;
	this->xt = momentX / area;
	this->yt = momentY / area;
}

const int ImageObject::getArea() const {
	return this->area;
}

const int ImageObject::getPerimeter() const {
	return this->perimeter;
}

const int ImageObject::getXt() const {
	return this->xt;
}

const int ImageObject::getYt() const {
	return this->yt;
}
