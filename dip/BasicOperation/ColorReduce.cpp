/**
@version: opencv-3.2.0-vc14
@patrick 2017-7-18
**/

#include <iostream>
#include <opencv.hpp>

using namespace std;
using namespace cv;

// ͨ�� at 
void colorReduce1(Mat img, int div)
{
	int r = img.rows, c = img.cols;
	int channels = img.channels();
	for (int i = 0; i < r; ++i){
		for (int j = 0; j < c; ++j){
			// ��ͨ��
			if (channels == 1){
				img.at<uchar>(i, j) = img.at<uchar>(i, j) / div * div;
			}
			// ��ͨ��
			else if (channels == 3){
				// BGR
				img.at<Vec3b>(i, j)[0] = img.at<Vec3b>(i, j)[0] / div * div;
				img.at<Vec3b>(i, j)[1] = img.at<Vec3b>(i, j)[1] / div * div;
				img.at<Vec3b>(i, j)[2] = img.at<Vec3b>(i, j)[2] / div * div;
			}
		}
	}
}

// ͨ��ָ��
void colorReduce2(Mat img, int div)
{
	int r = img.rows, c = img.cols;
	int channels = img.channels();
	for (int i = 0; i < r; ++i){
		// ��ͨ��
		if (channels == 1){
			uchar* pData = img.ptr<uchar>(i);
			for (int j = 0; j < c; ++j){
				pData[j] = pData[j] / div * div;
			}
		}
		// ��ͨ��
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

// ͨ��������
void colorReduce3(Mat img, int div)
{
	int channels = img.channels();
	// ��ͨ��
	if (channels == 1){
		for (MatIterator_<uchar> it = img.begin<uchar>(), end = img.end<uchar>(); it != end; ++it){
			*it = *it / div * div;
		}
	}
	// ��ͨ��
	else if (channels == 3){
		for (MatIterator_<Vec3b> it = img.begin<Vec3b>(), end = img.end<Vec3b>(); it != end; ++it){
			// BGR
			(*it)[0] = (*it)[0] / div * div;
			(*it)[1] = (*it)[1] / div * div;
			(*it)[2] = (*it)[2] / div * div;
		}
	}
}

// ��������ͼ�񣬿���һά���飬ͨ�������ַ����
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
				// ��ͨ��
				pData[i * step0 + j] = pData[i * step0 + j] / div * div;
			}
			else if (channels == 3){
				// ��ͨ��
				pData[i * step0 + j * step1 + 0 * eleSize1] = pData[i * step0 + j * step1 + 0 * eleSize1] / div * div;
				pData[i * step0 + j * step1 + 1 * eleSize1] = pData[i * step0 + j * step1 + 1 * eleSize1] / div * div;
				pData[i * step0 + j * step1 + 2 * eleSize1] = pData[i * step0 + j * step1 + 2 * eleSize1] / div * div;
			}
		}
	}
}

// ������ӳ���ϵ������ͼ��ģ�����ͨ��LUT������ӳ��
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
	// (��άͼ��)ÿ������ռ�ֽ��� = step[1] * cols
	cout << "step[0] = " << src.step[0] << endl;
	// (��άͼ��)ÿ���㣨Ԫ�أ���ռ�ֽ��� = elemSize
	cout << "step[1] = " << src.step[1] << endl;
	// ÿ��Ԫ����ռ�ֽ��� = eleSize1 * channels
	cout << "elemSize() = " << src.elemSize() << endl;
	// ÿ��ͨ����ռ�ֽ��� = depth / 8
	cout << "elemSize1() = " << src.elemSize1() << endl;

	Mat img = src.clone();
	double duration;
	duration = static_cast<double>(getTickCount());
	colorReduce1(img, 64);
	duration = static_cast<double>(getTickCount()) - duration;
	duration /= getTickFrequency();

	cout << "\nDebug������ʱ�䣺 " << endl;
	cout << "ͨ�� at ���� : " << duration << endl;

	img = src.clone();
	duration = static_cast<double>(getTickCount());
	colorReduce2(img, 64);
	duration = static_cast<double>(getTickCount()) - duration;
	duration /= getTickFrequency();
	cout << "ͨ��ָ����� : " << duration << endl;

	img = src.clone();
	duration = static_cast<double>(getTickCount());
	colorReduce3(img, 64);
	duration = static_cast<double>(getTickCount()) - duration;
	duration /= getTickFrequency();
	cout << "ͨ������������ : " << duration << endl;

	img = src.clone();
	duration = static_cast<double>(getTickCount());
	colorReduce4(img, 64);
	duration = static_cast<double>(getTickCount()) - duration;
	duration /= getTickFrequency();
	cout << "��������ͼ��ͨ�������ַ : " << duration << endl;

	img = src.clone();
	duration = static_cast<double>(getTickCount());
	colorReduce5(img, 64);
	duration = static_cast<double>(getTickCount()) - duration;
	duration /= getTickFrequency();
	cout << "������ӳ���ϵ��ͨ��LUT : " << duration << endl;

	namedWindow("res", WINDOW_NORMAL);
	imshow("res", img);

	waitKey();
	return 0;
}