#include <opencv2/opencv.hpp>

using namespace std;

//棒グラフの表示
void barGraph(int width, int height, cv::Mat hist, vector<int> &histVector)
{
	// 要素数
	int num = histVector.size();
	cout << "size of histVector: " << num << endl;
	// 最大要素
	int max = histVector[0];
	for (int i = 1; i < num; i++)
	{
		if (histVector[i] > max)
			max = histVector[i];
	}
	// または int max = *max_element( histVector.begin(), histVector.end() );
	// 棒グラフ配置設計
	double dw = (double)width / (num * 2 + 1); // 棒の幅と間隔を同じに
	cout << "size of bar width: " << dw << endl;
	double dh = (double)height / max; // 度数あたりの高さ画素数
	cout << "height unit: " << dh << endl;

	// 各棒の描画
	for (int i = 0; i < num; i++)
	{
		double x = dw * (2 * i + 1);			// 棒グラフ内描画用x座標
		double y = height - histVector[i] * dh; // 棒グラフ内描画用y座標
		cout << "i, count: " << i << ", " << histVector[i] << endl;
		cv::rectangle(hist, cv::Rect2d(x, y, dw, histVector[i] * dh), cv::Scalar(255, 0, 0), cv::LineTypes::FILLED);
	}
}

// 1ch画像の度数表を作成する関数
void calcHistogram(cv::Mat src, vector<int> &histVector)
{
	// 各画素値の頻度を算出
	int rows = src.rows, cols = src.cols;
	int binNum = histVector.size();
	for (int y = 0; y < rows; y++)
	{
		uchar *p = src.ptr(y);
		for (int x = 0; x < cols; x++)
		{
			histVector[p[x] * binNum / 256]++;
		}
	}
}

// カラー画像の輝度ヒストグラムを描画する関数
void hueHistogram(char **argv)
{
	cv::Mat hsv;
	cv::Mat matArray[3];
	cv::Mat image = cv::imread(argv[1]);
	if (image.empty())
	{
		std::cout << "画像読み込み失敗" << std::endl;
		return;
	}
	// 棒グラフ幅(default=6)
	int binWidth = atoi(argv[2]);
	// hueの取り出し
	cv::cvtColor(image, hsv, cv::ColorConversionCodes::COLOR_BGR2HSV); // RBG -> HSV
	cv::split(hsv, matArray);

	vector<int> histVector((int)(180 / binWidth)); // 度数表宣言
	calcHistogram(matArray[0], histVector);		   // hueの度数表作成

	// 棒グラフウィンドウ
	int width = atoi(argv[3]);
	int height = atoi(argv[4]);
	cv::Mat hist;
	hist.create(cv::Size(width, height), CV_8UC3); // カラー画像生成
	hist = cv::Scalar::all(255);				   // 全面白色で初期化
	barGraph(width, height, hist, histVector);	 // ヒストグラムを出力
	cv::imshow("Input", image);
	cv::imshow("Hue", matArray[0]);
	cv::imshow("Histgram", hist);
	cv::waitKey();
}

int main(int argc, char *argv[])
{
	// hueHistogram関数に渡す配列の宣言
	//引数省略時の入力画像，window幅，window高さのdefault設定込み
	char *args[5] = {argv[0], "sample.jpg", "6", "400", "300"};
	if (argc >= 2)
		args[1] = argv[1]; //引数での入力画像指定
	if (argc >= 3)
		args[2] = argv[2]; //引数でのbin幅指定
	if (argc >= 4)
		args[3] = argv[3]; //引数でのwindow幅指定
	if (argc >= 5)
		args[4] = argv[4]; //引数でのwindow高指定

	hueHistogram(args);

	return 0;
}
