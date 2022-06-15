#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

//画像ファイルの表示(プログラム実行時にファイル名を指定)
void readImage_cin()
{
	cout << "画像ファイル名を入力してください(例：hoge.jpg): ";

	string name;
	cin >> name;	//コンソールへの入力をnameに代入

	cv::Mat image;
	image = cv::imread(name);	//画像の読み込み

	if (image.empty())	//画像の読み込みに失敗したら終了
	{
		cout << "画像ファイルを読み込めませんでした．" << endl;
		return;
	}
	cv::imshow(name, image);	//imageに格納された画像を表示する
	cv::waitKey();
}

int main(int argc, char** argv)
{
	readImage_cin();
	return 0;
}
