#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace cv;

// グレースケール画像のヒストグラム平坦化によるコントラスト調整
void grayHistEqualize(char* filename)
{
	cv::Mat src = cv::imread(filename, cv::IMREAD_GRAYSCALE);
	cv::imshow("Input", src);

	// ルックアップテーブル
	// 今回は1つのルックアップテーブルの中でヒストグラムや累積ヒストグラムの計算を完結させる
	int LUT[256] = {};

	// (1) 画素値のヒストグラムを算出 
	for (int i = 0; i < src.rows; i++)
		for (int j = 0; j < src.cols; j++)
			LUT[src.ptr(i, j)[0]]++;

	 // (2) (1)のヒストグラムから累積ヒストグラムを算出 
	for (int i = 1; i < 256; i++)
		LUT[i] += LUT[i - 1];

	// (3) 累積ヒストグラムから階調変換用LUTを作成 
	for (int i = 0; i < 256; i++)
		LUT[i] = LUT[i] * 255.0 / LUT[255];

	// (4) 画像走査内でLUTを用いて階調変換を行う
	cv::Mat result = cv::Mat(src.size(), CV_8UC1);
	for (int i = 0; i < result.rows; i++)
		for (int j = 0; j < result.cols; j++)
			result.ptr(i, j)[0] = LUT[src.ptr(i, j)[0]];

	// 結果を表示する
	cv::imshow("Result", result);
	cv::waitKey();
}

int main(int argc, char* argv[])
{
	if (argc < 2) grayHistEqualize("sample.jpg");	//default画像
	else grayHistEqualize(argv[1]);
	return 0;
}