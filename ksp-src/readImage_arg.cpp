#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;

//画像ファイルの表示(プログラムの実行時引数でファイル名を指定)
void readImage_arg(char* name)
{
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
	if (argc < 2)
	{
		readImage_arg("sample.jpg");  //引数省略時のdefault設定 
	}
	else {
		readImage_arg(argv[1]);
	}
	return 0;
}
