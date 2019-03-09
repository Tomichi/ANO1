#include "Etalons.h"

Etalons::Etalons(std::string names) {
	this->x = this->y = this->z = 0.0;
	this->sumX = this->sumY = this->sumZ = 0.0;
	this->numberElements = 0;
	this->name = names;

}

void Etalons::set(ImageObject & object)  {
	this->sumX += object.F1;
	this->sumY += object.F2;
	this->numberElements += 1;
	this->x = static_cast<double>(this->sumX / this->numberElements);
	this->y = static_cast<double>(this->sumY / this->numberElements);
}

double Etalons::getX() {
	return this->x;
}

double Etalons::getY() {
	return this->y;
}

double Etalons::computeDist(ImageObject & object) {
	double res = (this->x - object.F1) * (this->x - object.F1);
	res += (this->y - object.F2) * (this->y - object.F2);

	return std::sqrt(res);
}

std::string Etalons::getName() {
	return this->name;
}

Etalons::~Etalons() {
	this->name.clear();
}
