/**
@version: opencv-3.2.0-vc14
@patrick 2017-6-26
**/

#include <opencv.hpp>

using namespace std;
using namespace cv;

int threshold_type = 0;
int max_type = 4;
int threshold_val = 0;
int max_val = 255;

char* window_name = "Treshold Demo";
char* trackbar_type = "Type ";
char* trackbar_value = "Value ";

Mat src, src_gray, dst;

void ThresholdUpdate(int, void*);

int main(int argc, char* argv[])
{
	if (argv[1] != NULL) {
		src = imread(argv[1], IMREAD_COLOR);
		if (src.empty()){
			fprintf(stderr, "Open Failed %s\n", argv[1]);
			return -1;
		}
	}

	// 转化为灰度图片
	cvtColor(src, src_gray, CV_RGB2GRAY);

	// 创建一个窗口
	namedWindow(window_name, CV_WINDOW_NORMAL);
	
	// 创建滑动条
	createTrackbar(trackbar_type, window_name, &threshold_type, max_type, ThresholdUpdate);
	createTrackbar(trackbar_value, window_name, &threshold_val, max_val, ThresholdUpdate);

	waitKey();
	return 0;
}

void ThresholdUpdate(int, void*)
{
	threshold(src_gray, dst, threshold_val, max_val, threshold_type);
	imshow(window_name, dst);
}