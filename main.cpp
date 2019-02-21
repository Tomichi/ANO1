#include <iostream>
#include <cmath>
#include <queue>
#include <opencv2/opencv.hpp>
#include "ImageObject.h"

const int TRESHOLD_VALUE = 127;
const uchar WHITE_PIXEL = 255;
const uchar BLACK_PIXEL = 0;

int getArea(int y, int x, cv::Mat & indexingImage) {
	cv::Mat copyImage = indexingImage.clone();
	std::queue<std::pair<int, int>> queue;
	const uchar CURRENT_COLOR = copyImage.at<uchar>(y, x);
	const uchar DIFFERENT_COLOR = 5;
	const int COLS = copyImage.cols, ROWS = copyImage.rows;
	int area = 1;
	queue.push({y, x});
	copyImage.at<uchar>(y, x) = DIFFERENT_COLOR;
	while (!queue.empty()) {
		auto & pair = queue.front();
		y = pair.first;
		x = pair.second;
		queue.pop();
		for (int xh = -1; xh < 2; xh++) {
			for (int yh = -1; yh < 2; yh++) {
				// Test image ranges
				if (x + xh < 0 || x + xh >= COLS || y + yh < 0 || y + yh >= ROWS) {
					continue;
				}

				if (copyImage.at<uchar>(y + yh, x + xh) == CURRENT_COLOR) {
					queue.push({y + yh, x + xh});
					copyImage.at<uchar>(y + yh, x + xh) = (uchar) DIFFERENT_COLOR;
					area += 1;
				}
			}
		}
	}

	return area;
}

int getPerimeter(int y, int x, cv::Mat & indexingImage) {
	cv::Mat copyImage = indexingImage.clone();
	std::queue<std::pair<int, int>> queue;
	const uchar CURRENT_COLOR = copyImage.at<uchar>(y, x);
	const uchar DIFFERENT_COLOR = 5;
	const int COLS = copyImage.cols, ROWS = copyImage.rows;
	int perimeter = 1;
	queue.push({y, x});
	copyImage.at<uchar>(y, x) = DIFFERENT_COLOR;
	while (!queue.empty()) {
		auto & pair = queue.front();
		y = pair.first;
		x = pair.second;
		queue.pop();
		bool isBorderPixel = false;
		for (int xh = -1; xh < 2; xh++) {
			for (int yh = -1; yh < 2; yh++) {
				// Test image ranges
				if (x + xh < 0 || x + xh >= COLS || y + yh < 0 || y + yh >= ROWS) {
					continue;
				}

				if (copyImage.at<uchar>(y + yh, x + xh) == CURRENT_COLOR) {
					queue.push({y + yh, x + xh});
					copyImage.at<uchar>(y + yh, x + xh) = (uchar) DIFFERENT_COLOR;
				}

				bool testIsFourDirection = (xh + yh) * (xh + yh) == 1;
				if (testIsFourDirection && copyImage.at<uchar>(y + yh, x + xh) == BLACK_PIXEL) {
					isBorderPixel = true;
				}
			}
		}

		perimeter += (isBorderPixel) ? 1 : 0;
	}

	return perimeter;
}

void computeFirstMoment(int y, int x, cv::Mat & indexingImage, long int & momentX, long int & momentY) {
	cv::Mat copyImage = indexingImage.clone();
	std::queue<std::pair<int, int>> queue;
	momentX = x, momentY = y;
	const uchar CURRENT_COLOR = copyImage.at<uchar>(y, x);
	const uchar DIFFERENT_COLOR = 5;
	const int COLS = copyImage.cols, ROWS = copyImage.rows;
	queue.push({y, x});
	copyImage.at<uchar>(y, x) = DIFFERENT_COLOR;
	while (!queue.empty()) {
		auto & pair = queue.front();
		y = pair.first;
		x = pair.second;
		queue.pop();
		for (int xh = -1; xh < 2; xh++) {
			for (int yh = -1; yh < 2; yh++) {
				// Test image ranges
				if (x + xh < 0 || x + xh >= COLS || y + yh < 0 || y + yh >= ROWS) {
					continue;
				}

				if (copyImage.at<uchar>(y + yh, x + xh) == CURRENT_COLOR) {
					queue.push({y + yh, x + xh});
					momentX += x + xh;
					momentY += y + yh;
					copyImage.at<uchar>(y + yh, x + xh) = (uchar) DIFFERENT_COLOR;
				}
			}
		}
	}

}

void floodFill(int y, int x, const int currentIndex, cv::Mat & indexingImage) {
	std::queue<std::pair<int, int>> queue;
	const auto CURRENT_COLOR = static_cast<uchar>(currentIndex);
	const int COLS = indexingImage.cols, ROWS = indexingImage.rows;
	indexingImage.at<uchar>(y, x) = (uchar) CURRENT_COLOR;
	queue.push({y, x});

	while (!queue.empty()) {
		auto & pairs = queue.front();
		queue.pop();
		y = pairs.first;
		x = pairs.second;

		// test 8 direction
		bool border = false;
		for (int xh = -1; xh < 2; xh++) {
			for (int yh = -1; yh < 2; yh++) {
				// Test image ranges
				if (x + xh < 0 || x + xh >= COLS || y + yh < 0 || y + yh >= ROWS) {
					continue;
				}

				if (indexingImage.at<uchar>(y + yh, x + xh) == WHITE_PIXEL) {
					indexingImage.at<uchar>(y + yh, x + xh) = CURRENT_COLOR;
					queue.push({y + yh, x + xh});
				}
			}
		}
	}
}

int main() {
	std::ios_base::sync_with_stdio(false);
	std::cout.sync_with_stdio(false);
	cv::Mat src_8uc1_img;
	src_8uc1_img = cv::imread("images/train.png", CV_LOAD_IMAGE_GRAYSCALE);
	const int cols = src_8uc1_img.cols, rows = src_8uc1_img.rows;

	for (int x = 0; x < cols; x++) {
		for (int y = 0; y < rows; y++) {
			src_8uc1_img.at<uchar>(y, x) = (src_8uc1_img.at<uchar>(y, x) >= TRESHOLD_VALUE) ? (uchar) 255 : (uchar) 0;
		}
	}

	cv::Mat indexingImage = src_8uc1_img.clone();
	std::vector<ImageObject> objects{};
	// floodFill
	int currentIndex = 20;
	int STEP = 10;
	int perimeter, area;
	long int momentX, momentY;
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			if (indexingImage.at<uchar>(y, x) == WHITE_PIXEL) {
				momentX = momentY = 0;
				floodFill(y, x, currentIndex, indexingImage);
				perimeter = getPerimeter(y, x, indexingImage);
				area = getArea(y, x, indexingImage);
				computeFirstMoment(y, x, indexingImage, momentX, momentY);
				objects.emplace_back(ImageObject(area, perimeter, momentX, momentY));
				currentIndex += STEP;
			}
		}
	}

	for (auto & object : objects) {
		std::cout << "Area " << object.getArea()
		          << " Perimeter " << object.getPerimeter()
		          << " Moment [xt,yt] = [" << object.getXt() << "," << object.getYt() << "]"
		          << "\n";
		indexingImage.at<uchar>(object.getYt(), object.getXt()) = WHITE_PIXEL;
	}

	objects.clear();

	cv::imshow("train after tresholding", src_8uc1_img);
	cv::imshow("train after flood fill", indexingImage);
	cv::waitKey(0); // wait until keypressed

	return 0;
}
