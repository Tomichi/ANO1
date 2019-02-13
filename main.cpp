#include <iostream>
#include <cmath>
#include <queue>
#include <opencv2/opencv.hpp>

void floodFill(int y, int x, int currentIndex, cv::Mat & indexingImage) {
	std::queue<std::pair<int, int>> queues;
	const uchar VISITED_PIXEL = 5, WHITE_PIXEL = 255;
	const int COLS = indexingImage.cols, ROWS = indexingImage.rows;
	indexingImage.at<uchar>(y, x) = VISITED_PIXEL;
	queues.push({y, x});
	while (!queues.empty()) {
		std::pair<uchar, uchar> pairs = queues.front();
		queues.pop();
		y = pairs.first;
		x = pairs.second;
		if (indexingImage.at<uchar>(y, x) == VISITED_PIXEL) {
			indexingImage.at<uchar>(y, x) = (uchar) currentIndex;

			// test 8 direction
			for (int xh = -1; xh < 2; xh++) {
				for (int yh = -1; yh < 2; yh++) {

					// Test image ranges
					if (x + xh < 0 || x + xh >= COLS || y+yh < 0 || y+yh >= ROWS) {
						continue;
					}

					if (indexingImage.at<uchar>(y + yh, x + xh) == WHITE_PIXEL) {
						indexingImage.at<uchar>(y + yh, x + xh) = VISITED_PIXEL;
						queues.push({y + yh, x + xh});
					}
				}
			}
		}
	}
}

int main() {
	std::cout.sync_with_stdio(false);
	cv::Mat src_8uc1_img;
	src_8uc1_img = cv::imread("images/train.png", CV_LOAD_IMAGE_GRAYSCALE);
	const int cols = src_8uc1_img.cols, rows = src_8uc1_img.rows;
	const int treshold = 127;

	for (int x = 0; x < cols; x++) {
		for (int y = 0; y < rows; y++) {
			src_8uc1_img.at<uchar>(y, x) = (src_8uc1_img.at<uchar>(y, x) >= treshold) ? (uchar) 255 : (uchar) 0;
		}
	}

	cv::Mat indexingImage = src_8uc1_img.clone();

	// floodFill
	int currentIndex = 20;
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			if (indexingImage.at<uchar>(y, x) == (uchar) 255) {
				floodFill(y, x, currentIndex, indexingImage);
				currentIndex += 10;
			}
		}
	}

	cv::imshow("train after tresholding", src_8uc1_img);
	cv::imshow("train after floodfill", indexingImage);
	cv::waitKey(0); // wait until keypressed

	return 0;
}
