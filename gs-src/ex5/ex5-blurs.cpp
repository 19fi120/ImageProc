#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace cv;

void blurs(char* fileName){
	cv::Mat image = cv::imread(fileName);
	if (image.empty()){
		std::cout << "画像読み込み失敗" << std::endl;
		return;
	}

    cv::Mat blur3x3, blur7x7, gaussian3x3, gaussian7x7;
    cv::blur(image, blur3x3, cv::Size(3, 3));
    cv::blur(image, blur7x7, cv::Size(7, 7));
    cv::GaussianBlur(image, gaussian3x3, cv::Size(3, 3), 2, 0);
    cv::GaussianBlur(image, gaussian7x7, cv::Size(7, 7), 2, 0);

	cv::imshow("Input", image);
    cv::imshow("Blur (3x3)", blur3x3);
    cv::imshow("Blur (7x7)", blur7x7);
    cv::imshow("Gaussian (3x3)", gaussian3x3);
    cv::imshow("Gaussian (7x7)", gaussian7x7);
	cv::waitKey();
}

int main(int argc, char* argv[])
{
	if (argc < 2) blurs("sample.jpg");	//default画像
	else blurs(argv[1]);
	return 0;
}