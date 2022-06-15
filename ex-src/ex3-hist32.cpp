#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;

//棒グラフの表示
void barGraph(int width, int height, cv::Mat hist, vector<int>& histVector)
{
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

	// 各棒の描画
	for(int i = 0; i < num; i++){
		double x = dw*(2*i+1);					// 棒グラフ内描画用x座標
		double y = height - histVector[i]*dh;	// 棒グラフ内描画用y座標
		cout << "i, count: " << i << ", " << histVector[i] << endl;
		cv::rectangle(hist, cv::Rect2d(x,y,dw,histVector[i]*dh), cv::Scalar(255, 0, 0), CV_FILLED);
	}
}

// 1ch画像の度数表を作成する関数
void calcHistogram(cv::Mat src, vector<int>& histVector){
	// 各画素値の頻度を算出
	int rows = src.rows, cols = src.cols;
	int binNum = histVector.size();
	for (int y = 0; y < rows; y++){
		uchar* p = src.ptr(y);
		for (int x = 0; x < cols; x++)		{
			histVector[p[x]*binNum/256]++;
		}
	}
}

// カラー画像の輝度ヒストグラムを描画する関数
void calcAndDrawHistogram(char** argv){
	cv::Mat gray;
	cv::Mat image = cv::imread(argv[1]);
	if (image.empty()){
		std::cout << "画像読み込み失敗" << std::endl;
		return;
	}
	cv::cvtColor(image, gray, CV_BGR2GRAY); // 3ch -> 1ch
	vector<int> histVector(32);	// 度数表宣言
	calcHistogram(gray, histVector);	// 度数表作成

	// 棒グラフウィンドウ
	int width = atoi(argv[2]);
	int height = atoi(argv[3]);
	cv::Mat hist;
	hist.create(cv::Size(width, height), CV_8UC3);	// カラー画像生成
	hist = cv::Scalar::all(255);	// 全面白色で初期化
	barGraph(width, height, hist, histVector);	// ヒストグラムを出力
	cv::imshow("Input", image);
	cv::imshow("Grayscale", gray);
	cv::imshow("Histgram", hist);
	cv::waitKey();
}

int main(int argc, char* argv[])
{
	// calcAndDrawHistogram関数に渡す配列の宣言
	//引数省略時の入力画像，window幅，window高さのdefault設定込み
	char* args[4] = {argv[0],"sample.jpg","400","300"};
	if (argc >= 2) args[1] = argv[1];	//引数での入力画像指定
	if (argc >= 3) args[2] = argv[2];	//引数でのwindow幅指定
	if (argc >= 4) args[3] = argv[3];	//引数でのwindow高指定

	calcAndDrawHistogram(args);

	return 0;
}
