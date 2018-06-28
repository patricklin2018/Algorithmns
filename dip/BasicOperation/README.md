# 图像基本操作

## 1. 像素处理

### 1.1 通过 at 进行访问

```cpp
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
```

### 1.2 通过指针进行访问

```cpp
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
```

### 1.3 通过迭代器

```cpp
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
```

### *1.4 对于连续图像，通过计算地址进行访问

前三种为常用的基本方法。后两种为特例情况。

对于连续图像，可以将图像看成一维数组，进行地址计算访问，是效率最高的方法。

当然，不像以前用 `IplImage` 需要考虑连续性问题，现在内存分配基本都是连续的，除了提取 `ROI` 是非连续的。

其中，对于 `Mat` 中 `step` 的概念如下二图：

* 对于二维存储 

![](https://github.com/PatrickLin1993/DIP/blob/master/BasicOperation/pics/step1.png)

* 对于三维存储（按面存储）

![](https://github.com/PatrickLin1993/DIP/blob/master/BasicOperation/pics/step2.png)

```cpp
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
			// 单通道			
			if (channels == 1){
				pData[i * step0 + j] = pData[i * step0 + j] / div * div;
			}
			// 三通道
			else if (channels == 3){
				pData[i * step0 + j * step1 + 0 * eleSize1] = pData[i * step0 + j * step1 + 0 * eleSize1] / div * div;
				pData[i * step0 + j * step1 + 1 * eleSize1] = pData[i * step0 + j * step1 + 1 * eleSize1] / div * div;
				pData[i * step0 + j * step1 + 2 * eleSize1] = pData[i * step0 + j * step1 + 2 * eleSize1] / div * div;
			}
		}
	}
}
```

### *1.5 对于有映射关系来处理图像的，可以通过LUT来进行映射处理

对于具有映射关系处理图像的，可以通过LUT(LookUpTable)，进行映射处理.

```cpp
void colorReduce5(Mat img, int div)
{
	Mat LookUpTable(1, 256, CV_8UC1);
	uchar* pTable = LookUpTable.data;
	for (int i = 0; i < 256; ++i){
		pTable[i] = i / div * div;
	}
	LUT(img, LookUpTable, img);
}
```

### 总结

[CPP 代码](https://github.com/PatrickLin1993/DIP/blob/master/BasicOperation/ColorReduce.cpp)

![](/pics/res1.png)

实验结论：

* 通过 `at` 可随机访问，但效率最低。
* 通过`指针访问`进行效率很高，但需注意指针安全。
* 通过`迭代器`进行访问次之，但是最安全。
* 对于连续图像，可以通过`地址计算`进行访问，效率奇高。
* 对于处理具有映射关系的，使用`LUT`不仅直观简单，而且效率更佳。
