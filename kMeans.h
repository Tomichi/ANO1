#pragma once

#include <tuple>
#include <cmath>
#include <random>
#include <string>

struct Point {
	double x = 0, y = 0, z = 0;

	explicit Point(double x_coor = 0, double y_coor = 0, double z_coor = 0) {
		x = x_coor;
		y = y_coor;
		z = z_coor;
	}

	Point() {
		x = y = z = 0.0;
	}
};

class kMeans {
	private:
		std::vector<Point> points;
		int number_of_clusters, iterations;;
		std::vector<Point> clusters;
		std::vector<int> assign_points_to_cluster;
		double euclideanDistance(const Point & point, const Point & cluster);
	public:
		kMeans(int clusters, int iterations);
		~kMeans();
		void addPoint(Point point);
		void runkMeans();
		std::string neighbourCluster(const Point & point);
};
