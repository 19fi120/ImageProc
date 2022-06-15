#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;

//動画ファイルの表示
void readVideo()
{
	cv::VideoCapture cap("sora.avi");	//動画ファイルの読み込み

	if (cap.isOpened() == false)	//動画ファイルを正しく読み込めたか
	{
		cout << "動画ファイルを読み込めませんでした．" << endl;
		return;
	}

	while (true)
	{
		cv::Mat frame;
		cap >> frame;	//動画からの画像を取得

		if (frame.data == NULL)	//動画からの画像を全て読み込んだら終了
			break;

		cv::imshow("Output", frame);

		cv::waitKey(33);	//引数の数値を変化させると…？
	}
}

int main(int argc, char** argv)
{
	readVideo();
	return 0;
}