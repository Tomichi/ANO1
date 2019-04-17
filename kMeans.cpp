#include "kMeans.h"

kMeans::kMeans(const int clusters, const int iterations) {
	this->number_of_clusters = clusters;
	this->iterations = iterations;
}

kMeans::~kMeans() {
	points.clear();
	assign_points_to_cluster.clear();
	clusters.clear();
}

void kMeans::addPoint(Point point) {
	this->points.emplace_back(point);
	this->assign_points_to_cluster.emplace_back(0);
}

double kMeans::euclideanDistance(const Point & point, const Point & cluster) {
	;
	double result = 0;
	result += (cluster.x - point.x) * (cluster.x - point.x);
	result += (cluster.y - point.y) * (cluster.y - point.y);
	result += (cluster.z - point.z) * (cluster.z - point.z);

	return std::sqrt(result);
}

void kMeans::runkMeans() {
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(this->points.begin(), this->points.end(), g);
	for (int k = 0; k < this->number_of_clusters; ++k) {
		this->clusters.push_back(this->points[k]);
	}
	double min, current_dist;
	int min_cluste_index;
	for (int iteration = 0; iteration < this->iterations; iteration++) {
		for (int point_index = 0; point_index < static_cast<int>(this->points.size()); point_index++) {
			min = INT_MAX;
			min_cluste_index = 0;
			for (int i = 0; i < this->number_of_clusters; i++) {
				current_dist = this->euclideanDistance(this->points[point_index], this->clusters[i]);
				if (current_dist < min) {
					min = current_dist;
					min_cluste_index = i;
				}
			}
			this->assign_points_to_cluster[point_index] = min_cluste_index;
		}
		for (int i = 0; i < this->number_of_clusters; i++) {
			int numbers = 0;
			this->clusters[i].x = this->clusters[i].y = this->clusters[i].z = 0;
			for (int point_index = 0; point_index < static_cast<int>(points.size()); point_index++) {
				if (this->assign_points_to_cluster[point_index] != i) continue;
				++numbers;
				this->clusters[i].x += points[point_index].x;
				this->clusters[i].y += points[point_index].y;
				this->clusters[i].z += points[point_index].z;
			}
			this->clusters[i].x /= numbers;
			this->clusters[i].y /= numbers;
			this->clusters[i].z /= numbers;
		}
	}
}

std::string kMeans::neighbourCluster(const Point & point) {
	double min = INT_MAX;
	int min_index = 0;
	for (int i = 0; i < static_cast<int>(this->clusters.size()); i++) {
		double current_dist = euclideanDistance(point, this->clusters[i]);
		if (current_dist < min) {
			min = current_dist;
			min_index = i;
		}
	}
	return std::to_string(min_index);
}
