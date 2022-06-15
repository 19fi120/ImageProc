#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

Mat posterize(Mat src)
{
    cv::Mat dst(cv::Size(src.cols, src.rows), CV_8UC3);
    for (int j = 0; j < src.rows; j++) {
        // y ⾏⽬の先頭画素へのポインタを取得
        uchar *p = src.ptr(j);
        uchar *q = dst.ptr(j);
        for (int i = 0; i < src.cols; i++) {
            // j⾏⽬,i列⽬の画素の処理
            if (p[i] < 64) q[i] = 32;
            else if (p[i] < 128) q[i] = 96;
            else if (p[i] < 192) q[i] = 160;
            else q[i] = 224;
        }
    }
    return dst;
}

//画像ファイルの表示(プログラムの実行時引数でファイル名を指定)
void posterizeAny(char* name)
{
	cv::Mat image, imageArray[3], posterizeImage;
	image = cv::imread(name);	//画像の読み込み

	if (image.empty())	//画像の読み込みに失敗したら終了
	{
		cout << "画像ファイルを読み込めませんでした．" << endl;
		return;
	}
    cv::split(image, imageArray);
    imageArray[0] = posterize(imageArray[0]);
    imageArray[1] = posterize(imageArray[1]);
    imageArray[2] = posterize(imageArray[2]);
    cv::merge(imageArray, 3, posterizeImage); 

	cv::imshow(name, image);	//imageに格納された画像を表示する
    cv::imshow(name, posterizeImage);
	cv::waitKey();
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		posterizeAny("sample.jpg");  //引数省略時のdefault設定 
	}
	else {
		posterizeAny(argv[1]);
	}
	return 0;
}