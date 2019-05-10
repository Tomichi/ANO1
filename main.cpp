#include <iostream>
#include <cmath>
#include <queue>
#include <opencv2/opencv.hpp>
#include "Etalons.h"
#include "kMeans.h"
#include "backprop.h"



#define KMEANS false
#define ETALONS false
#define NEURAL true
const uchar WHITE_PIXEL = 255;
const uchar BLACK_PIXEL = 0;

std::string objects_name[4]= {"Ctverec", "Hvezda", "Obdelnik", "Kolecko"};

void train(NN* nn)
{
	int n = 16;
	auto ** trainingSet = new double * [n];
	double f1,f2,f3,a1,a2,a3,a4;
	std::ifstream neuralDataFile;
	neuralDataFile.open("dataclear.txt");
	if (!neuralDataFile) {
		std::cout << "Unable to open file";
		return;
	}

	for ( int i = 0; i < n; i++ ) {
		trainingSet[i] = new double[nn->n[0] + nn->n[nn->l - 1]];
		neuralDataFile >> f1 >> f2 >> f3 >> a1 >> a2 >> a3 >> a4;
		int key = 0;
		for (auto value : {f1,f2,f3,a1,a2,a3,a4}) {
			trainingSet[i][key] = value;
			++key;
		}
	}
	neuralDataFile.close();

	double error = 1.0;
	int i = 0;
	while(error > 0.001) {
		setInput( nn, trainingSet[i%n] );
		feedforward( nn );
		error = backpropagation( nn, &trainingSet[i%n][nn->n[0]] );
		i++;
		//std::cout << "err="<< error<< "\n";
	}
	std::cout << "err="<< error<< "\n";
	std::cout <<"("<< i <<" iterations)\n";

	for ( int i = 0; i < n; i++ ) {
		delete [] trainingSet[i];
	}
	delete [] trainingSet;
}

double computePerpendicularityFeature(ImageObject & obj, cv::Mat & indexingImage) {
	int x = 0, y = 0, x_xh = 0, y_yh = 0;
	double degree_rad = 0, new_x = 0, new_y = 0, max_x = 0, max_y = 0, area_AR = INT_MAX,
			min_x = INT_MAX, min_y = INT_MAX;
	for (int degree = 0; degree <= 90; degree += 5) {
		degree_rad = M_PI * (degree / 180.0);
		cv::Mat copyImage = indexingImage.clone();
		x = static_cast<int>(obj.xt);
		y = static_cast<int>(obj.yt);
		std::queue<std::pair<int, int>> queue;
		const uchar CURRENT_COLOR = copyImage.at<uchar>(y, x);
		const uchar DIFFERENT_COLOR = 255;
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
						copyImage.at<uchar>(y + yh, x + xh) = (uchar) DIFFERENT_COLOR;
						x_xh = x + xh - static_cast<int>(obj.xt), y_yh = y + yh - static_cast<int>(obj.yt);

						new_x = cos(degree_rad) * (x_xh) - sin(degree_rad) * (y_yh);
						new_y = sin(degree_rad) * (x_xh) + cos(degree_rad) * (y_yh);
						min_x = std::min(min_x, new_x);
						min_y = std::min(min_y, new_y);
						max_x = std::max(max_x, new_x);
						max_y = std::max(max_y, new_y);
					}
				}
			}
		}

		double tmp_area = (max_x - min_x) * (max_y - min_y);
		area_AR = std::min(tmp_area, area_AR);
		min_x = min_y = INT_MAX;
		max_y = max_x = 0;
	}

	return area_AR;
}

void computeFeatures(ImageObject & obj, cv::Mat & indexingImage) {
	cv::Mat copyImage = indexingImage.clone();
	double momentX2, momentY2, moment11, perpendicularity;
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

	perpendicularity = computePerpendicularityFeature(obj, indexingImage);

	obj.setFeatures(momentX2, momentY2, moment11, perpendicularity);
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

void tresholdingImage(cv::Mat & input_image, const int TRESHOLD_VALUE) {
	const int cols = input_image.cols, rows = input_image.rows;
	for (int x = 0; x < cols; x++) {
		for (int y = 0; y < rows; y++) {
			input_image.at<uchar>(y, x) = (input_image.at<uchar>(y, x) >= TRESHOLD_VALUE) ? (uchar) 255 : (uchar) 0;
		}
	}
}

void indexingImage(cv::Mat & input_image, std::vector<ImageObject> & image_objects) {
	const int cols = input_image.cols, rows = input_image.rows;
	int current_index = 20;
	int STEP = 10;
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			if (input_image.at<uchar>(y, x) == WHITE_PIXEL) {
				floodFill(y, x, current_index, input_image);
				auto imageObject = computeMoment(y, x, input_image);
				computeFeatures(imageObject, input_image);
				image_objects.emplace_back(imageObject);
				current_index += STEP;
			}
		}
	}
}

int main() {
	std::ios_base::sync_with_stdio(false);
	cv::Mat src_8uc1_img, src_8uc1_img_test;
	src_8uc1_img = cv::imread("images/train04.png", CV_LOAD_IMAGE_GRAYSCALE);
	src_8uc1_img_test = cv::imread("images/test04.png", CV_LOAD_IMAGE_GRAYSCALE);
	const int TRESHOLD_VALUE = 127;
	// train set
	tresholdingImage(src_8uc1_img, TRESHOLD_VALUE);
	// test set
	tresholdingImage(src_8uc1_img_test, TRESHOLD_VALUE);

	cv::Mat train_indexing_image = src_8uc1_img.clone(),
			test_indexing_image = src_8uc1_img_test.clone();
	std::vector<ImageObject> train_objects{}, test_objects{};
	// floodFill
	indexingImage(train_indexing_image, train_objects);
	indexingImage(test_indexing_image, test_objects);

	std::cout << "Train dataset \n";
	for (auto & object : train_objects) {
		object.printsImageObject();
		train_indexing_image.at<uchar>(static_cast<int>(object.yt), static_cast<int>(object.xt)) = WHITE_PIXEL;
	}


#if defined(NEURAL) && NEURAL == true
	NN * nn = createNN(3, 4, 4);
	train(nn);
	double* in = new double[nn->n[0]];
	//test(nn, 100);

#endif

#if defined(ETALONS) && ETALONS == true
	std::vector<Etalons> etalons;
	for (auto & name : objects_name) {
		etalons.emplace_back(name);
	}

	for (int i = 0; i < train_objects.size(); i++) {
		etalons[i / 4].set(train_objects[i]);
	}


	for (auto & etalon: etalons) {
		std::cout << "x =" << etalon.getX() << " y=" << etalon.getY() << " " << " z=" << etalon.getZ() << " "
		          << etalon.getName() << "\n";
	}
#endif
#if defined(KMEANS) && KMEANS == true
	kMeans KMeans(4, 50);
	for (const auto & object: train_objects) {
		KMeans.addPoint(Point(object.F1, object.F2, object.F3));
	}

	KMeans.runkMeans();
#endif
	std::cout << "Test dataset \n";
	for (auto & object : test_objects) {
		object.printsImageObject();
		test_indexing_image.at<uchar>(static_cast<int>(object.yt), static_cast<int>(object.xt)) = WHITE_PIXEL;

#if defined(KMEANS) && KMEANS == true
		const auto index = KMeans.neighbourCluster(Point(object.F1, object.F2, object.F3));

		cv::Point centerPoint = cv::Point(static_cast<int>(object.xt) - 20, static_cast<int>(object.yt));

		cv::putText(test_indexing_image, index, centerPoint, cv::FONT_HERSHEY_PLAIN, 1.0,
		            cv::Scalar(255), 1);
#endif

#if defined(ETALONS) && ETALONS == true
		double min = etalons[0].computeDist(object);
		int min_index = 0;
		for (int i = 1; i < etalons.size(); i++) {
			if (etalons[i].computeDist(object) < min) {
				min = etalons[i].computeDist(object);
				min_index = i;
			}
		}

		cv::Point centerPoint = cv::Point(static_cast<int>(object.xt) - 20, static_cast<int>(object.yt));
		cv::putText(test_indexing_image, etalons[min_index].getName(), centerPoint, cv::FONT_HERSHEY_PLAIN, 1.0,
		            cv::Scalar(255), 1.0);

		std::cout << etalons[min_index].getName() << "\n";
#endif
#if defined(NEURAL) && NEURAL == true
		int key = 0;
		for(auto value: {object.F1, object.F2, object.F3}) {
			in[key] =value;
			key++;
		}
		setInput( nn, in, true );
		feedforward( nn );
		int output = getOutput( nn, true );
		cv::Point centersPoint = cv::Point(static_cast<int>(object.xt) - 20, static_cast<int>(object.yt));

		cv::putText(test_indexing_image, objects_name[output], centersPoint, cv::FONT_HERSHEY_PLAIN, 1.0,
		            cv::Scalar(255), 1);

#endif

	}

	train_objects.clear();
	test_objects.clear();
#if defined(ETALONS) && ETALONS == true
	etalons.clear();
#endif

#if defined(NEURAL) && NEURAL == true
	delete [] in;
	releaseNN(nn);
#endif

	//cv::imshow("result of tresholding", src_8uc1_img);
	//cv::imshow("result of test tresholding", src_8uc1_img_test);
	cv::imshow("result of flood fill train", train_indexing_image);
	cv::imshow("result of flood fill", test_indexing_image);
	cv::waitKey(0); // wait until keypressed

	return 0;
}
