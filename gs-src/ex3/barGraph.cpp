#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace cv;

//棒グラフの表示
void barGraph(cv::Mat hist, vector<int>& histVector)
{
	// グラフ幅
	int width = hist.cols;
	// グラフ高さ
	int height = hist.rows;
	// 要素数
	int num = histVector.size();
	cout << "size of histVector: " << num << endl;
	// 最大要素
	int max = histVector[0];
	for(int i = 1; i < num; i++){
		if (histVector[i] > max) max = histVector[i];
	}
	// または int max = *max_element( histVector.begin(), histVector.end() );
	// 棒グラフ配置設計
	double dw = (double)width/(num*2+1);	// 棒の幅と間隔を同じに
	cout << "size of bar width: " << dw << endl;
	double dh = (double)height/max;				// 度数あたりの高さ画素数
	cout << "height unit: " << dh << endl;
	
	hist = cv::Scalar::all(255);	// 全面白色で初期化
	// 各棒の描画
	for(int i = 0; i < num; i++){
		double x = dw*(2*i+1);					// 棒グラフ内描画用x座標
		double y = height - histVector[i]*dh;	// 棒グラフ内描画用y座標
		cout << "i, count: " << i << ", " << histVector[i] << endl;
		cv::rectangle(hist, cv::Rect2d(x,y,dw,histVector[i]*dh), cv::Scalar(255, 0, 0), cv::FILLED);
	}
	return;
}

int main(int argc, char** argv)
{
	char* args[2] = {"600","300"};	   //引数省略時のdefaultのwindow幅とwindow高さ
	if (argc >= 2) args[0] = argv[1];	//引数でのwindow幅指定
	if (argc >= 3) args[1] = argv[2];	//引数でのwindow高指定
	cv::Mat hist(cv::Size(atoi(args[0]),atoi(args[1])), CV_8UC3);	// カラー画像生成
	vector<int> histVector = { 3, 7, 2, 8, 0, 5 };
	barGraph(hist,histVector);
	cv::imshow("bar graph", hist);
	cv::waitKey();
	return(0);
}