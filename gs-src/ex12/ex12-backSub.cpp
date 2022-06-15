#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace cv;

void backSub(int cameraNumber){
    cv::Mat background;
    cv::VideoCapture cap(cameraNumber);
    if (cap.isOpened() == false){
        std::cout << “カメラ入力失敗" << std::endl;
        return;
    }
    switch (cv::waitKey(33))
	{
		case 's':
			background = cap;
			cout << "メディアンフィルターのカーネルサイズ: " << ksize << endl;
			break;
		case 'd':
			ksize = max(1, ksize - 2);
			cout << "メディアンフィルターのカーネルサイズ: " << ksize << endl;
			break;
		case 'q':
			return;
		}
	
	}
}

int main(int argc, char* argv[]) {
    if (argc < 2) backSub(0); //引数省略時のdefault設定
    else backSub(atoi(argv[1])); // カメラ番号
return 0;
}