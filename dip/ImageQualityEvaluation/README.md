# 图像质量评价的两种客观方法

## 1. PSNR

### 1.1 原理

峰值信噪比(Peank Signal to Noise Ratio - PSNR)，基于误差敏感的客观评价指标。运算速度比 `MSSIM` 快很多，使用最为广泛。但是由于其没有考虑到人眼的视觉特性（人眼通常聚焦在图像的局部，且对亮度对比更为敏感等），因此有时会出现与主观评价不一致的结果。

![](/pics/mse.png)

>mse 为 图像 I1 和 I2 的均方误差
>c 为 通道数， i j 分别为行数和列数

![](/pics/psnr.png)

>MAX 为像素阶数，及 2 的深度次方，一般为 255

最后单位为 `dB`。图像失真越少，则 PSNR 越大。若完全一样则 PNSR 等于 0。

### 1.2 代码

```cpp
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
```

## 2. MSSIM

### 2.1 原理

结构相似性(Structural Similarity - SSIM)，与基于误差敏感度(error sensitivity)的质量评价方法不同，SSIM 考虑到了人眼视觉特性，以亮度对比，对比度对比，结构对比为基础进行图像质量评价。

具体数学原理：

[论文 - Image quality assessment: From error visibility to structural similarity](https://wenku.baidu.com/view/c383c03283c4bb4cf7ecd12b.html)

[SSIM 算法原理](http://blog.csdn.net/ecnu18918079120/article/details/60149864)

### 2.2 代码

```cpp
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
```

## 实验结果

[CPP 代码](https://github.com/PatrickLin1993/DIP/blob/master/ImageQualityEvaluation/ImageQualityEvaluation.cpp)

![](/pics/res.png)

## 参考

[官方教程 - Video Input with OpenCV and similarity measurement](http://docs.opencv.org/3.2.0/d5/dc4/tutorial_video_input_psnr_ssim.html)