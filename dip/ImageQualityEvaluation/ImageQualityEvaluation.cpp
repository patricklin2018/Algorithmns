/**
@version: opencv-3.2.0-vc14
@patrick 2017-7-20
**/

#include <iostream>
#include <opencv.hpp>

using namespace std;
using namespace cv;

double getPSNR(const Mat& I1, const Mat& I2)
{
	Mat diff;
	// |I1 - I2|
	absdiff(I1, I2, diff);	
	// |I1 - I2| ^ 2
	diff = diff.mul(diff);
	
	// 各通道元素相加
	Scalar s = sum(diff);
	double see = s.val[0] + s.val[1] + s.val[2];
	
	// mse
	if (see <= 1e-10){
		return 0;
	}
	double mse = see / (double)(I1.total() * I1.channels());

	// psnr
	double psnr = 10 * log10(255 * 255 / mse);

	return psnr;
}

Scalar getMSSIM(const Mat& I1, const Mat& I2)
{
	// 一般地取 k1, k2 以下值
	double k1 = 0.01, k2 = 0.03;
	// c1 = (k1 * L)^2, c2 = (k2 * L)^2
	double c1 = powl(k1 * 255, 2), c2 = powl(k2 * 255, 2);

	// 为了后面的乘法计算等，加大深度
	Mat ix, iy;
	int d = CV_32F;
	I1.convertTo(ix, d);
	I2.convertTo(iy, d);
	
	// 计算均值 miu, 用 11*11 高斯加权窗体，1.5 标准差
	Mat miu_x, miu_y;
	GaussianBlur(ix, miu_x, Size(11, 11), 1.5);
	GaussianBlur(iy, miu_y, Size(11, 11), 1.5);
	// ux^2, uy^2, ux * uy 
	Mat miu_x2 = miu_x.mul(miu_x);
	Mat miu_y2 = miu_y.mul(miu_y);
	Mat miu_xy = miu_x.mul(miu_y);

	// 计算方差 sigma，同样采用高斯加权
	Mat ix2 = ix.mul(ix);
	Mat iy2 = iy.mul(iy);
	Mat ixy = ix.mul(iy);
	// sigma_x^2, sigma_y^2, sigma_x * y
	Mat sigma_x2, sigma_y2, sigma_xy;
	GaussianBlur(ix2, sigma_x2, Size(11, 11), 1.5);
	sigma_x2 -= miu_x2;
	GaussianBlur(iy2, sigma_y2, Size(11, 11), 1.5);
	sigma_y2 -= miu_y2;
	GaussianBlur(ixy, sigma_xy, Size(11, 11), 1.5);
	sigma_xy -= miu_xy;

	// ssim
	Mat ssim;
	divide((2 * miu_xy + c1).mul(2 * sigma_xy + c2), (miu_x2 + miu_y2 + c1).mul(sigma_x2 + sigma_y2 + c2), ssim);

	// mssim
	Scalar mssim = mean(ssim);

	return mssim;
}

void colorReduce(Mat img, int div)
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
	Mat src = imread("test.jpg");
	namedWindow("src", WINDOW_NORMAL);
	imshow("src", src);

	Mat dst1 = src.clone();
	colorReduce(dst1, 32);
	namedWindow("dst1", WINDOW_NORMAL);
	imshow("dst1", dst1);

	Mat dst2 = src.clone();
	colorReduce(dst2, 64);
	namedWindow("dst2", WINDOW_NORMAL);
	imshow("dst2", dst2);

	Mat dst3 = src.clone();
	colorReduce(dst3, 127);
	namedWindow("dst3", WINDOW_NORMAL);
	imshow("dst3", dst3);

	cout << "PSNR: " << setprecision(3) << endl;
	cout << "Src and Src   = " << getPSNR(src, src)   << "dB" << endl;
	cout << "Src and blur1 = " << getPSNR(src, dst1) << "dB" << endl;
	cout << "Src and blur2 = " << getPSNR(src, dst2) << "dB" << endl;
	cout << "Src and blur3 = " << getPSNR(src, dst3) << "dB" << endl;

	cout << "\nMSSIM: " << endl;
	Scalar res = getMSSIM(src, src);
	cout << "Src and Src   = " << "[R] = " << res.val[2] * 100 << "%, [G] = " << res.val[1] * 100 << "%, [B] = " << res.val[0] * 100 << "%" << endl;
	res = getMSSIM(src, dst1);
	cout << "Src and blur1 = " << "[R] = " << res.val[2] * 100 << "%, [G] = " << res.val[1] * 100 << "%, [B] = " << res.val[0] * 100 << "%" << endl;
	res = getMSSIM(src, dst2);
	cout << "Src and blur2 = " << "[R] = " << res.val[2] * 100 << "%, [G] = " << res.val[1] * 100 << "%, [B] = " << res.val[0] * 100 << "%" << endl;
	res = getMSSIM(src, dst3);
	cout << "Src and blur3 = " << "[R] = " << res.val[2] * 100 << "%, [G] = " << res.val[1] * 100 << "%, [B] = " << res.val[0] * 100 << "%" << endl;

	waitKey(0);
	return 0;
}