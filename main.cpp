#include <iostream>
#include <cmath>
#include <queue>
#include <opencv2/opencv.hpp>
#include "ImageObject.h"

const int TRESHOLD_VALUE = 127;
const uchar WHITE_PIXEL = 255;
const uchar BLACK_PIXEL = 0;

void computeFeatures(ImageObject & obj, cv::Mat & indexingImage) {
	cv::Mat copyImage = indexingImage.clone();
	double momentX2, momentY2, moment11;
	auto x = static_cast<int>(obj.xt), y = static_cast<int>(obj.yt);
	std::queue<std::pair<int, int>> queue;
	momentY2 = moment11 = momentX2 = 0;
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
					momentX2 += (x + xh - (obj.xt)) * (x + xh - (obj.xt));
					momentY2 += (y + yh - (obj.yt)) * (y + yh - (obj.yt));
					moment11 += (x + xh - (obj.xt)) * (y + yh - (obj.yt));
					copyImage.at<uchar>(y + yh, x + xh) = (uchar) DIFFERENT_COLOR;
				}
			}
		}
	}
	obj.setFeatures(momentX2, momentY2, moment11);
}

ImageObject computeMoment(int y, int x, cv::Mat & indexingImage) {
	cv::Mat copyImage = indexingImage.clone();
	long int momentX, momentY, area, perimeter;
	std::queue<std::pair<int, int>> queue;
	momentX = x;
	momentY = y;
	perimeter = 0;
	area = 1;
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
		bool isBorderPixel = false;
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
					area += 1;
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

	return {area, perimeter, momentX, momentY};

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
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			if (indexingImage.at<uchar>(y, x) == WHITE_PIXEL) {
				floodFill(y, x, currentIndex, indexingImage);
				auto imageObject = computeMoment(y, x, indexingImage);
				computeFeatures(imageObject, indexingImage);
				objects.emplace_back(imageObject);
				currentIndex += STEP;
			}
		}
	}

	for (auto & object : objects) {
		std::cout << "Area " << object.area
		          << " Perimeter " << object.perimeter
		          << " Moment [xt,yt] = [" << object.xt << "," << object.yt << "]"
		          << " F1 = " << object.F1
		          << " F2 = " << object.F2
		          << "\n";
		indexingImage.at<uchar>(static_cast<int>(object.yt), static_cast<int>(object.xt)) = WHITE_PIXEL;
	}

	objects.clear();

	cv::imshow("result of tresholding", src_8uc1_img);
	cv::imshow("result of flood fill", indexingImage);
	cv::waitKey(0); // wait until keypressed

	return 0;
}
