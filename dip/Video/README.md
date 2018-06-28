# 视频操作
## 1. 视频输入

[官方教程 - Video Input with OpenCV and similarity measurement](http://docs.opencv.org/3.2.0/d5/dc4/tutorial_video_input_psnr_ssim.html)

下面为读取视频的每一帧，进行阈值处理。

```cpp
/**
@version: opencv-3.2.0-vc14
@patrick 2017-7-20
**/

#include <iostream>
#include <opencv.hpp>

using namespace std;
using namespace cv;

double GetMeanThreshold(Mat img, int graylvl)
{
	if (img.empty()) {
		return 0.0;
	}

	// 1. 计算图像灰度直方图
	if (img.channels() == 3) {
		cvtColor(img, img, CV_RGB2GRAY);
	}
	Mat hist_gray;
	float range[] = { 0, 255 };
	const float* hist_range = range;
	calcHist(&img, 1, 0, Mat(), hist_gray, 1, &graylvl, &hist_range);

	// 2. 归一化直方图，得出直方图每一项的概率
	normalize(hist_gray, hist_gray, 1, 0, NORM_L1);

	// 3. 直方图的横坐标乘于概率，然后求和得出阈值
	double bin_width = (range[1] - range[0]) / graylvl;
	double thres = 0.0;
	for (int i = 0; i < graylvl; ++i) {
		thres += hist_gray.at<float>(i) * ((i + 0.5) * bin_width);
	}

	return thres;
}

int main(int argv, char** argc)
{
	VideoCapture video("testvideo.mp4");
	if (!video.isOpened()){
		cout << "fail to open the file !" << endl;
	}

	// 获取视频帧数
	long totalFrameNumber = video.get(CV_CAP_PROP_FRAME_COUNT);
	cout << "总帧数 = " << totalFrameNumber << endl;

	// 获取帧率
	double fps = video.get(CV_CAP_PROP_FPS);
	cout << "FPS = " << fps << endl;

	// 循环读取帧
	const char* cSrcVideo = "SrcVideo";
	const char* cDstVideo = "DstVideo";
	namedWindow(cSrcVideo, CV_WINDOW_NORMAL);
	namedWindow(cDstVideo, CV_WINDOW_NORMAL);
	Mat frame;
	double delay = 1000 / fps;
	while (1){
		video >> frame;
		if (frame.empty()){
			break;
		}

		imshow(cSrcVideo, frame);

		cvtColor(frame, frame, CV_RGB2GRAY);
		double thres = GetMeanThreshold(frame, 255);
		threshold(frame, frame, thres, thres, 3);

		imshow(cDstVideo, frame);

		waitKey(delay);
	}

	waitKey(0);
	return 0;
}
```

效果 :

![](/pics/res1.png)

## 2. 视频输出

[官方教程 - Creating a video with OpenCV](http://docs.opencv.org/3.2.0/d7/d9e/tutorial_video_write.html)

opencv 支持如下编码格式

```
CV_FOURCC('P', 'I', 'M', '1') = MPEG-1 codec
CV_FOURCC('M', 'J', 'P', 'G') = motion-jpeg codec
CV_FOURCC('M', 'P', '4', '2') = MPEG-4.2 codec 
CV_FOURCC('D', 'I', 'V', '3') = MPEG-4.3 codec 
CV_FOURCC('D', 'I', 'V', 'X') = MPEG-4 codec 
CV_FOURCC('U', '2', '6', '3') = H263 codec 
CV_FOURCC('I', '2', '6', '3') = H263I codec 
CV_FOURCC('F', 'L', 'V', '1') = FLV1 codec
```

以下代码，为读取视频，转化并输出为单颜色通道的视频：

```cpp
/**
@version: opencv-3.2.0-vc14
@patrick 2017-7-20
**/

#include <iostream>
#include <opencv.hpp>

using namespace std;
using namespace cv;

double GetMeanThreshold(Mat img, int graylvl)
{
	if (img.empty()) {
		return 0.0;
	}

	// 1. 计算图像灰度直方图
	if (img.channels() == 3) {
		cvtColor(img, img, CV_RGB2GRAY);
	}
	Mat hist_gray;
	float range[] = { 0, 255 };
	const float* hist_range = range;
	calcHist(&img, 1, 0, Mat(), hist_gray, 1, &graylvl, &hist_range);

	// 2. 归一化直方图，得出直方图每一项的概率
	normalize(hist_gray, hist_gray, 1, 0, NORM_L1);

	// 3. 直方图的横坐标乘于概率，然后求和得出阈值
	double bin_width = (range[1] - range[0]) / graylvl;
	double thres = 0.0;
	for (int i = 0; i < graylvl; ++i) {
		thres += hist_gray.at<float>(i) * ((i + 0.5) * bin_width);
	}

	return thres;
}

int main(int argc, char *argv[])
{
	VideoCapture inputVideo("testvideo.mp4");
	if (!inputVideo.isOpened()){
		cout << "Could not open the input video !" << endl;
		return -1;
	}

	// 获取编码格式（int 格式）
	int ex = static_cast<int>(inputVideo.get(CAP_PROP_FOURCC));
	// 通过位操作，将 int 格式的编码格式转换成 char 格式
	char EXT[] = { (char)(ex & 0XFF), (char)((ex & 0XFF00) >> 8), (char)((ex & 0XFF0000) >> 16), (char)((ex & 0XFF000000) >> 24), 0 };
	
	cout << "视频编码 ： " << EXT << endl;

	// 打开输出视频
	Size S = Size((int)inputVideo.get(CAP_PROP_FRAME_WIDTH),
		(int)inputVideo.get(CAP_PROP_FRAME_HEIGHT));
	VideoWriter outputVideo("outputVideo.avi", CV_FOURCC('M', 'J', 'P', 'G'), inputVideo.get(CAP_PROP_FPS), S, true);
	if (!outputVideo.isOpened()){
		cout << "Could not open the output video for write !" << endl;
		return -1;
	}

	Mat src, res;
	Mat mv[3];
	long iFrame = 0;
	while (1) {
		inputVideo >> src;       
		if (src.empty()){ 
			break; 
		}
		cout << "进行第 " << iFrame << " 帧编码" << endl;
		split(src, mv);
		for (int i = 0; i < 3; ++i){
			if (i != 2){
				mv[i] = Mat::zeros(S, mv[0].type());
			}
		}
		merge(mv, 3, res);

		outputVideo << res;

		iFrame++;
	}
	cout << "Finished writing" << endl;

	return 0;
}
```

效果：

![](/pics/res2.png)