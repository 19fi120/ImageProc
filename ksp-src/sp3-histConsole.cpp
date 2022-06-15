#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;

// 1ch画像の度数表を作成する関数
void calcHistogram(cv::Mat src, vector<int>& histVector){
	// 各画素値の頻度を算出
	int rows = src.rows, cols = src.cols;
	for (int y = 0; y < rows; y++){
		uchar* p = src.ptr(y);
		for (int x = 0; x < cols; x++)		{
			histVector[p[x]]++;
		}
	}
}
// ヒストグラムをコンソールへ出力する関数
void printHistogram(vector<int>& histVector){
	int length = histVector.size();
	for (int i = 0; i < length; i++){
		std::cout << i << " : " << histVector[i] << 		std::endl;
	}
}
// 1ch画像のヒストグラムをコンソールへ出力する関数
void calcAndPrintHistogram(){
	cv::Mat mat = cv::imread("sample.jpg");
	if (mat.empty()){
		std::cout << "画像読み込み失敗" << std::endl;
		return;
	}
	cv::cvtColor(mat, mat, CV_BGR2GRAY); // 3ch -> 1ch
	vector<int> histVector(256);	// 度数表宣言
	calcHistogram(mat, histVector);	// 度数表作成
	printHistogram(histVector);	// ヒストグラムを出力
	cv::imshow("mat", mat);
	cv::waitKey();
}
int main(){
	calcAndPrintHistogram();
	return 0;
}
