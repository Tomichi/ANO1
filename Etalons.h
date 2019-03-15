#pragma once

#include "ImageObject.h"

class Etalons {
	public:
		Etalons(std::string name);
		~Etalons();
		void set(ImageObject & object);
		double getX();
		double getY();
		double getZ();
		std::string getName();
		double computeDist(ImageObject & object);
	private:
		double x,y,z, sumX, sumY, sumZ;
		int numberElements;
		std::string name{};

};
