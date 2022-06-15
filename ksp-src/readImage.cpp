#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;

//画像ファイルの表示
void readImage()
{
	cv::Mat image = cv::imread("sample.jpg");	//画像の読み込み

	if (image.empty())	//画像が正しく読み込めたのかを確認
	{
		cout << "画像ファイルを読み込めませんでした．" << endl;
		return;
	}

	cv::imshow("Output", image);	//画像の表示

	cv::waitKey();	//入力があるまで待機
}

int main(int argc, char** argv)
{
	readImage();
	return 0;
}
