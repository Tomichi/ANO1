#include <iostream>
#include <cmath>
#include <queue>
#include <string>
#include <iomanip>
#include <opencv2/opencv.hpp>


class Cell {
	public:
		int width, height;
		Cell(const int w, const int h): width(w), height(h) { };
};

class Block {
	public:
		int width, height, num_of_cells_x, num_of_cells_y;

		Block(const int w, const int h, const Cell c) : width(w), height(h) {
			this->num_of_cells_x = static_cast<int>(std::round(width / c.width));
			this->num_of_cells_y = static_cast<int>(std::round(height / c.height));
		}
};

class ImageInfo {
	public:
		int width, height, num_of_cells_x, num_of_cells_y, num_of_block_x, num_of_block_y;

		ImageInfo(const int w, const int h, const Block b, const Cell c): width(w), height(h) {
			this->num_of_cells_x = static_cast<int>(std::round(width / c.width));
			this->num_of_cells_y = static_cast<int>(std::round(height / c.height));
			this->num_of_block_x = static_cast<int>(std::round(width / b.width));
			this->num_of_block_y = static_cast<int>(std::round(height / b.height));
		}
};

void preproccessingHog(cv::Mat & source, cv::Mat & feature, cv::Mat & cell, const int PINS) {
	double degree_of_pins = 180.0 / PINS;
	const int height = source.rows, width = source.cols;
	double fx, fy, angle, size;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (y != height - 1 && x != width - 1) {
				// compute fx
				fx = static_cast<double>(source.at<uchar>(y, x + 1));
				fx -= static_cast<double>(source.at<uchar>(y, x));

				// compute fy
				fy = static_cast<double>(source.at<uchar>(y + 1, x));
				fy -= static_cast<double>(source.at<uchar>(y, x));
				angle = std::atan2(fy, fx);
				size = std::sqrt(fx * fx + fy * fy);
				feature.at<cv::Vec2d>(y, x) = cv::Vec2d(angle, size);
				angle = (angle * 180) / M_PI;
				angle += (angle < 0.0) ? 180 : 0;
				cell.at<uchar>(y, x) = (static_cast<int>(std::floor(angle / degree_of_pins)) % PINS);
				//std::cout << fx << " " << fy << " " << feature.at<cv::Vec2d>(y, x)[1] <<" " << size  << " " << (int)cell.at<uchar>(y, x) << "\n";
			} else {
				cell.at<uchar>(y, x) = 0;
				feature.at<cv::Vec2d>(y, x) = cv::Vec2d(0.0, 0.0);
			}


		}
	}
}

std::vector <std::vector<std::vector < double>> >

createCellsVector(const ImageInfo & info, const int PINS) {
	std::vector < std::vector < std::vector < double > > > cells_vector;
	cells_vector.resize(info.num_of_cells_y);
	for (int y = 0; y < info.num_of_cells_y; ++y) {
		cells_vector[y].resize(info.num_of_cells_x);
		for (int x = 0; x < info.num_of_cells_x; ++x) {
			cells_vector[y][x].resize(PINS);
			for (int p = 0; p < PINS; ++p) {
				cells_vector[y][x][p] = 0.0;
			}
		}
	}
	return cells_vector;
}

int main() {
	std::ios_base::sync_with_stdio(false);
	const int DATA_SETS = 800;
	for (int number = 0; number < 10; number++) {
		for (int dataSet = 0; dataSet < DATA_SETS; dataSet++) {
			cv::Mat src_8uc1_img, src_8uc1_img_test;
			std::stringstream image_path;
			image_path << "/Users/tomichi/ClionProjects/ANO1/MNIST/testing/" << number << "/" << std::setfill('0')
			           << std::setw(4) << dataSet << ".png";

			src_8uc1_img = cv::imread(image_path.str(), CV_LOAD_IMAGE_GRAYSCALE);
			const int height = src_8uc1_img.rows, width = src_8uc1_img.cols;
			cv::Mat f_features(height, width, CV_64FC2);
			cv::Mat cells(height, width, CV_8UC1);

			// number of pins
			const int PINS = 9;
			// fill f-direction
			preproccessingHog(src_8uc1_img, f_features, cells, PINS);

			const int BLOCK_SIZE = 14;
			Cell cell(BLOCK_SIZE / 2, BLOCK_SIZE / 2);
			Block block(BLOCK_SIZE, BLOCK_SIZE, cell);
			ImageInfo imageInfo(width, height, block, cell);

			// create cells_vector
			auto cells_vector = createCellsVector(imageInfo, PINS);

			// fill cells_vector histogram
			int start_x, end_x, start_y, end_y;
			for (int y = 0; y < imageInfo.num_of_cells_y; y++) {
				start_y = cell.height * y;
				end_y = std::min(start_y + cell.height, height);
				for (int x = 0; x < imageInfo.num_of_cells_x; x++) {
					start_x = cell.width * x;
					end_x = std::min(start_x + cell.width, width);
					//std::cout << "cells (" << start_x << "," << start_y << "), (" << end_x << "," << end_y << ")"<< "\n";
					for (int yp = start_y; yp < end_y; yp++) {
						for (int xp = start_x; xp < end_x; xp++) {
							//if (f_features.at<cv::Vec2d>(yp, xp)[1] <= 0.0000) continue;
							auto p = (int) cells.at<uchar>(yp, xp);
							//std::cout << p << " " << f_features.at<cv::Vec2d>(yp, xp)[1] << "\n";
							cells_vector[y][x][p] += f_features.at<cv::Vec2d>(yp, xp)[1];
						}
					}
				}
			}
			double normalize_number;
			int start_by, end_by, start_bx, end_bx;
			for (int y = 0; y < imageInfo.num_of_block_y; y++) {
				start_y = block.height * y;
				end_y = std::min(start_y + block.height, height);
				for (int x = 0; x < imageInfo.num_of_block_x; x++) {
					start_x = block.width * x;
					end_x = std::min(start_x + block.width, width);
					normalize_number = 0;
					for (int yp = start_y; yp < end_y; yp++) {
						for (int xp = start_x; xp < end_x; xp++) {
							auto feature = f_features.at<cv::Vec2d>(yp, xp);
							normalize_number += feature[1];
						}
					}
					if (normalize_number == 0) continue;
					start_by = y * block.num_of_cells_y;
					end_by = std::min(start_by + block.num_of_cells_y, imageInfo.num_of_cells_y);
					start_bx = x * block.num_of_cells_x;
					end_bx = std::min(start_bx + block.num_of_cells_x, imageInfo.num_of_cells_x);
					//std::cout << "(" <<start_by << "," << end_by << ") x (" << start_bx << "," << end_bx << ")\n";
					for (int yp = start_by; yp < end_by; yp++) {
						for (int xp = start_bx; xp < end_bx; xp++) {
							for (int i = 0; i < PINS; i++) {
								cells_vector[yp][xp][i] /= normalize_number;
							}
						}
					}
				}
			}

			std::cout << number << ",";
			for (int y = 0; y < imageInfo.num_of_cells_y; ++y) {
				for (int x = 0; x < imageInfo.num_of_cells_x; ++x) {
					for (int p = 0; p < PINS; ++p) {
						std::cout << std::fixed << std::setprecision(5) << cells_vector[y][x][p] << ", ";
					}
				}
			}
			std::cout << "\n";
			cells_vector.clear();

			//cv::imshow("img", src_8uc1_img);
			//cv::waitKey(50);
			//std::cout << "\n";
		}
	}
	//cv::waitKey(0); // wait until keypressed

	return 0;
}
