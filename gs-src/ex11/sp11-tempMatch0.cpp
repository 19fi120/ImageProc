#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;

void templateMatch(char* fileName, char* templName){
	cv::Mat image = cv::imread(fileName);
	if (image.empty()){
		std::cout << "入力画像読み込み失敗" << std::endl;
		return;
	}
	cv::Mat templ = cv::imread(templName);
	if (image.empty()){
		std::cout << "テンプレート画像読み込み失敗" << std::endl;
		return;
	}

	cv::Mat simula;
	cv::Mat simulaout;
	cv::Mat simulaoutth;
	int thld = 200; // 0～255 の値域に対する閾値

//	cv::cvtColor(image,gray,CV_BGR2GRAY);
	cv::matchTemplate(image,templ,simula, cv::TM_SQDIFF_NORMED);
	// または CV_TM_CCORR_NORMED

	// 評価データsimula の32Fから8Uへの型変換
	// 引数は，出力Mat，型(ビット深度)，
	// α(スケールファクタ), β(足される値)
	simula.convertTo(simulaout,CV_8U,-255,255);
	// -255,255 は0～1を 255～0 に値域変換
	// 255,0 は0～1を 0～255 に値域変換
	// 127.5, 128 は-1～1を 0～255 に値域変換

	// 入力Mat,出力Mat,閾値,maxVal,閾値処理手法
	cv::threshold(simulaout,simulaoutth,thld,255,cv::THRESH_BINARY);
	cv::imshow("Input", image);
	cv::imshow("Template", templ);
	cv::imshow("Output1", simulaout);
	cv::imshow("Output2", simulaoutth);
	cv::waitKey();
    return;
}

int main(int argc, char* argv[])
{
	if (argc < 3){
		cout << "agrs: filename template" << endl;
	} else {
		templateMatch(argv[1], argv[2]);
	}
	return 0;
}