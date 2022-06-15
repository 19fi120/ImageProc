#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;

// 引数の画像ファイルを読み込みネガポジ反転して表示する
void negaPosi(char* fileName){
	cv::Mat image = cv::imread(fileName);
	if (image.empty()){
		std::cout << "画像読み込み失敗" << std::endl;
		return;
	}
	cv::Mat result = ~image;
	cv::Mat roi(result, cv::Rect(200, 150, 250, 150));	//領域の切り抜き
	roi = ~roi;
	cv::imshow("Input", image);
	cv::imshow("Result", result);
	cv::waitKey();
}

int main(int argc, char* argv[])
{
	if (argc < 2) negaPosi("sample.jpg");	//default画像
	else negaPosi(argv[1]);

	return 0;
}
