#include <iostream>
#include <cmath>
#include <queue>
#include <opencv2/opencv.hpp>


int main() {
	std::ios_base::sync_with_stdio(false);
	cv::Mat src_8uc1_img, src_8uc1_img_test;
	src_8uc1_img = cv::imread("/Users/tomichi/ClionProjects/ANO1/images/hog_test.png", CV_LOAD_IMAGE_GRAYSCALE);
	const int height = src_8uc1_img.rows, width = src_8uc1_img.cols;
	cv::Mat f_features(src_8uc1_img.rows, src_8uc1_img.cols, CV_64FC2);
	cv::Mat cells(src_8uc1_img.rows, src_8uc1_img.cols, CV_8UC1);

	// fill f-direction
	// number of pins
	const int PINS = 9;
	double degree_of_pins = 180.0 / PINS;
	double fx, fy, angle, size;
	for (int y = 0; y < height - 1; y++) {
		for (int x = 0; x < width - 1; x++) {
			// compute fx
			fx = static_cast<double>(src_8uc1_img.at<uchar>(y, x + 1));
			fx -= static_cast<double>(src_8uc1_img.at<uchar>(y, x));

			// compute fy
			fy = static_cast<double>(src_8uc1_img.at<uchar>(y + 1, x));
			fy -= static_cast<double>(src_8uc1_img.at<uchar>(y, x));
			angle = std::atan2(fy, fx);
			size = std::sqrt(fx * fx + fy * fy);
			f_features.at<cv::Vec2d>(y, x) = cv::Vec2d(angle, size);
			angle = (angle * 180) / M_PI;
			angle += (angle < 0.0) ? 180 : 0;
			cells.at<uchar>(y, x) = static_cast<int>(std::floor(angle / degree_of_pins));
			//std::cout << fx << " " << fy << " " << angle << " " << (int)cells.at<uchar>(y,x) << "\n";
		}
	}

	const int BLOCK_SIZE = 16, CELL_SIZE = BLOCK_SIZE / 2;
	const int NUM_OF_CELLS_Y = (int) std::round(height / CELL_SIZE),
			NUM_OF_CELLS_X = (int) std::round(width / CELL_SIZE),
			NUM_OF_BLOCKS_Y = (int) std::round(height / BLOCK_SIZE),
			NUM_OF_BLOCKS_X = (int) std::round(width / BLOCK_SIZE);
	const int CELLS_IN_BLOCK = 2;
	// create cells_vector
	std::vector<std::vector<std::vector<double> > > cells_vector;
	cells_vector.resize(NUM_OF_CELLS_Y);
	for (int y = 0; y < NUM_OF_CELLS_Y; ++y) {
		cells_vector[y].resize(NUM_OF_CELLS_X);
		for (int x = 0; x < NUM_OF_CELLS_X; ++x) {
			cells_vector[y][x].resize(PINS);
			for (int p = 0; p < PINS; ++p) {
				cells_vector[y][x][p] = 0.0;
			}
		}
	}

	// fill cells_vector histogram
	int start_x, end_x, start_y, end_y;
	for (int y = 0; y < NUM_OF_CELLS_Y; y++) {
		start_y = CELL_SIZE * y;
		end_y = std::min(start_y + CELL_SIZE, height);
		for (int x = 0; x < NUM_OF_CELLS_X; x++) {
			start_x = CELL_SIZE * x;
			end_x = std::min(start_x + CELL_SIZE, width);
			//std::cout << "cells (" << start_x << "," << start_y << "), (" << end_x << "," << end_y<< ")" << "\n";
			for (int yp = start_y; yp < end_y; yp++) {
				for (int xp = start_x; xp < end_x; xp++) {
					cells_vector[y][x][(int)cells.at<uchar>(yp, xp)] += f_features.at<cv::Vec2d>(yp, xp)[1];
				}
			}
			//for (int p = 0; p < PINS; ++p) {
			//	std::cout << cells_vector[y][x][p] << " ";
			//}
			//std::cout << "\n";
		}
	}
	double normalize_number;
	int start_by, end_by, start_bx, end_bx;
	for (int y = 0; y < NUM_OF_BLOCKS_Y; y++) {
		start_y = BLOCK_SIZE * y;
		end_y = std::min(start_y + BLOCK_SIZE, height);
		for (int x = 0; x < NUM_OF_BLOCKS_X; x++) {
			start_x = BLOCK_SIZE * x;
			end_x = std::min(start_x + BLOCK_SIZE, width);
			normalize_number = 0;
			for (int yp = start_y; yp < end_y; yp++) {
				for (int xp = start_x; xp < end_x; xp++) {
					auto feature = f_features.at<cv::Vec2d>(yp, xp);
					normalize_number += feature[1];
				}
			}
			//std::cout << "normalize " << normalize_number << "\n";
			if (normalize_number == 0) continue;
			start_by = y * CELLS_IN_BLOCK;
			end_by = std::min(start_by+CELLS_IN_BLOCK, NUM_OF_CELLS_Y);
			start_bx = x * CELLS_IN_BLOCK;
			end_bx = std::min(start_bx+CELLS_IN_BLOCK, NUM_OF_CELLS_X);
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

	for (int y = 0; y < NUM_OF_CELLS_Y; ++y) {
		for (int x = 0; x < NUM_OF_CELLS_X; ++x) {
			for (int p = 0; p < PINS; ++p) {
				std::cout << cells_vector[y][x][p] << " ";
			}
			std::cout << "\n";
		}
	}

	//cv::waitKey(0); // wait until keypressed

	return 0;
}
