/**
@version: opencv-3.2.0-vc14
@patrick 2017-7-18
**/

#include <iostream>
#include <opencv.hpp>

using namespace std;
using namespace cv;

// 通过 at 
void colorReduce1(Mat img, int div)
{
	int r = img.rows, c = img.cols;
	int channels = img.channels();
	for (int i = 0; i < r; ++i){
		for (int j = 0; j < c; ++j){
			// 单通道
			if (channels == 1){
				img.at<uchar>(i, j) = img.at<uchar>(i, j) / div * div;
			}
			// 三通道
			else if (channels == 3){
				// BGR
				img.at<Vec3b>(i, j)[0] = img.at<Vec3b>(i, j)[0] / div * div;
				img.at<Vec3b>(i, j)[1] = img.at<Vec3b>(i, j)[1] / div * div;
				img.at<Vec3b>(i, j)[2] = img.at<Vec3b>(i, j)[2] / div * div;
			}
		}
	}
}

// 通过指针
void colorReduce2(Mat img, int div)
{
	int r = img.rows, c = img.cols;
	int channels = img.channels();
	for (int i = 0; i < r; ++i){
		// 单通道
		if (channels == 1){
			uchar* pData = img.ptr<uchar>(i);
			for (int j = 0; j < c; ++j){
				pData[j] = pData[j] / div * div;
			}
		}
		// 三通道
		else if (channels == 3){
			Vec3b* pData = img.ptr<Vec3b>(i);
			for (int j = 0; j < c; ++j){
				// BGR
				pData[j][0] = pData[j][0] / div * div;
				pData[j][1] = pData[j][1] / div * div;
				pData[j][2] = pData[j][2] / div * div;
			}
		}
	}
}

// 通过迭代器
void colorReduce3(Mat img, int div)
{
	int channels = img.channels();
	// 单通道
	if (channels == 1){
		for (MatIterator_<uchar> it = img.begin<uchar>(), end = img.end<uchar>(); it != end; ++it){
			*it = *it / div * div;
		}
	}
	// 三通道
	else if (channels == 3){
		for (MatIterator_<Vec3b> it = img.begin<Vec3b>(), end = img.end<Vec3b>(); it != end; ++it){
			// BGR
			(*it)[0] = (*it)[0] / div * div;
			(*it)[1] = (*it)[1] / div * div;
			(*it)[2] = (*it)[2] / div * div;
		}
	}
}

// 对于连续图像，看成一维数组，通过计算地址遍历
void colorReduce4(Mat img, int div)
{
	if (!img.isContinuous()){
		return;
	}
	uchar* pData = img.data;
	int step0 = img.step;
	int step1 = img.step[1];
	int eleSize1 = img.elemSize1();
	int r = img.rows, c = img.cols;
	int channels = img.channels();
	for (int i = 0; i < r; ++i){
		for (int j = 0; j < c; ++j){
			if (channels == 1){
				// 单通道
				pData[i * step0 + j] = pData[i * step0 + j] / div * div;
			}
			else if (channels == 3){
				// 三通道
				pData[i * step0 + j * step1 + 0 * eleSize1] = pData[i * step0 + j * step1 + 0 * eleSize1] / div * div;
				pData[i * step0 + j * step1 + 1 * eleSize1] = pData[i * step0 + j * step1 + 1 * eleSize1] / div * div;
				pData[i * step0 + j * step1 + 2 * eleSize1] = pData[i * step0 + j * step1 + 2 * eleSize1] / div * div;
			}
		}
	}
}

// 对于有映射关系来处理图像的，可以通过LUT来进行映射
void colorReduce5(Mat img, int div)
{
	Mat LookUpTable(1, 256, CV_8UC1);
	uchar* pTable = LookUpTable.data;
	for (int i = 0; i < 256; ++i){
		pTable[i] = i / div * div;
	}
	LUT(img, LookUpTable, img);
}

int main(int argv, char** argc)
{
	Mat src = imread("test.jpg", IMREAD_COLOR);
	namedWindow("src", WINDOW_NORMAL);
	imshow("src", src);
	
	cout << "rows = " << src.rows << endl;
	cout << "cols = " << src.cols << endl;
	cout << "channels = " << src.channels() << endl;
	cout << "dimmension = " << src.dims << endl;
	cout << "step = " << src.step << endl;
	// (二维图像)每个行所占字节数 = step[1] * cols
	cout << "step[0] = " << src.step[0] << endl;
	// (二维图像)每个点（元素）所占字节数 = elemSize
	cout << "step[1] = " << src.step[1] << endl;
	// 每个元素所占字节数 = eleSize1 * channels
	cout << "elemSize() = " << src.elemSize() << endl;
	// 每个通道所占字节数 = depth / 8
	cout << "elemSize1() = " << src.elemSize1() << endl;

	Mat img = src.clone();
	double duration;
	duration = static_cast<double>(getTickCount());
	colorReduce1(img, 64);
	duration = static_cast<double>(getTickCount()) - duration;
	duration /= getTickFrequency();

	cout << "\nDebug下运行时间： " << endl;
	cout << "通过 at 遍历 : " << duration << endl;

	img = src.clone();
	duration = static_cast<double>(getTickCount());
	colorReduce2(img, 64);
	duration = static_cast<double>(getTickCount()) - duration;
	duration /= getTickFrequency();
	cout << "通过指针遍历 : " << duration << endl;

	img = src.clone();
	duration = static_cast<double>(getTickCount());
	colorReduce3(img, 64);
	duration = static_cast<double>(getTickCount()) - duration;
	duration /= getTickFrequency();
	cout << "通过迭代器遍历 : " << duration << endl;

	img = src.clone();
	duration = static_cast<double>(getTickCount());
	colorReduce4(img, 64);
	duration = static_cast<double>(getTickCount()) - duration;
	duration /= getTickFrequency();
	cout << "对于连续图像，通过计算地址 : " << duration << endl;

	img = src.clone();
	duration = static_cast<double>(getTickCount());
	colorReduce5(img, 64);
	duration = static_cast<double>(getTickCount()) - duration;
	duration /= getTickFrequency();
	cout << "对于有映射关系，通过LUT : " << duration << endl;

	namedWindow("res", WINDOW_NORMAL);
	imshow("res", img);

	waitKey();
	return 0;
}