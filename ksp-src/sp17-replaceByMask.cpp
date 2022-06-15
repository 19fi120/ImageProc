#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;

void replaceByMask(char *arg[])
{
	cv::Mat imageA = cv::imread(arg[1]);
	if (imageA.empty())
	{
		std::cout << "画像A読み込み失敗" << std::endl;
		return;
	}
	cv::Mat imageB = cv::imread(arg[2]);
	if (imageB.empty())
	{
		std::cout << "画像B読み込み失敗" << std::endl;
		return;
	}
	cv::Mat imageM = cv::imread(arg[3]);
	if (imageM.empty())
	{
		std::cout << "マスク画像読み込み失敗" << std::endl;
		return;
	}
	cv::Mat srcA, srcB, mask;
	cv::resize(imageA, srcA, cv::Size(640, 480)); // BGRの3チャンネル画像
	cv::resize(imageB, srcB, cv::Size(640, 480)); // BGRの3チャンネル画像
	cv::cvtColor(imageM, mask, cv::ColorConversionCodes::COLOR_BGR2GRAY);
	cv::resize(mask, mask, cv::Size(640, 480)); // グレースケールの1チャンネル画像

	cv::Mat result(cv::Size(srcA.cols, srcA.rows), CV_8UC3);

	for (int y = 0; y < srcA.rows; y++)
	{
		uchar *p = srcA.ptr(y);
		uchar *q = srcB.ptr(y);
		uchar *m = mask.ptr(y);
		uchar *r = result.ptr(y);
		for (int x = 0; x < srcA.cols; x++)
		{ //マスクは1チャネル，他は3チャネル
			r[x * 3] = p[x * 3] * m[x] / 255 + q[x * 3] * (255 - m[x]) / 255;
			r[x * 3 + 1] = p[x * 3 + 1] * m[x] / 255 + q[x * 3 + 1] * (255 - m[x]) / 255;
			r[x * 3 + 2] = p[x * 3 + 2] * m[x] / 255 + q[x * 3 + 2] * (255 - m[x]) / 255;
		}
	}

	cv::imshow("image A", srcA);		   //画像Aの表示
	cv::imshow("image B", srcB);		   //画像Bの表示
	cv::imshow("mask", mask);			   //マスク画像の表示
	cv::imshow("Blending Result", result); //ブレンディング結果画像の表示
	cv::waitKey();						   //入力があるまで待機
}

int main(int argc, char *argv[])
{
	// alphaBlend関数に渡す配列の宣言
	char *args[4] = {argv[0], "../image/imageA.jpg", "../image/imageB.jpg", "../image/mask.jpg"};
	if (argc >= 2)
		args[1] = argv[1]; // 画像A
	if (argc >= 3)
		args[2] = argv[2]; // 画像B
	if (argc >= 4)
		args[3] = argv[3]; // 画像B

	replaceByMask(args);

	return 0;
}
