#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;

// 引数の画像ファイルを読み込みメディアンフィルタを施す
// カーネルサイズ(default 3×3)はキー入力「u」で+2,キー入力「d」で-2(最少値1)に変更
void medBlurKeyControl(char* fileName)
{
	cv::Mat image = cv::imread(fileName);
	if (image.empty()){
		std::cout << "画像読み込み失敗" << std::endl;
		return;
	}
	int ksize = 3;
	cv::Mat medBlur;
	while (true)
	{
	switch (cv::waitKey(33))
	{
		case 'u':
			ksize = ksize + 2;
			cout << "メディアンフィルターのカーネルサイズ: " << ksize << endl;
			break;
		    case 'd':
			ksize = max(1, ksize - 2);
			cout << "メディアンフィルターのカーネルサイズ: " << ksize << endl;
			break;
		    case 'q':
			return;
		}
	cv::medianBlur(image, medBlur, ksize);
	cv::imshow("Input", image);
	cv::imshow("Median Blur", medBlur);
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2) medBlurKeyControl("sample.jpg");	//default画像
	else medBlurKeyControl(argv[1]);
	return 0;
}