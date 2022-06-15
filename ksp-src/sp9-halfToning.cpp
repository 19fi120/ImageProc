// https://wikiwiki.jp/akamonzaka/cv/samples/half_toning を改造
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;

bool capture(cv::Mat img, string fmt){
    static int i = 0;
    stringstream now;
    now << i;
    string fullname;
    if(i < 10)
	fullname = "capture-0" + now.str() + fmt;
    else
	fullname = "capture-" + now.str() + fmt;
    bool chk = cv::imwrite(fullname, img);
    i++;
    return chk;
}

cv::Mat floydSteinberg(cv::Mat_<unsigned char> a){
    cv::namedWindow("FloydSteinberg", cv::WINDOW_AUTOSIZE);
    cv::Mat_<unsigned char> result = a.clone();
    for(int y = 0; y < a.rows; y++){
	for(int x = 0; x < a.cols; x++){
	    int f = a(y, x);
	    int e;
	    if(f > 128){
		e = f-255;
		f = 255;
	    }else{
		e = f;
		f = 0;
	    }
	    result(y, x) = f;
	    if(x != a.cols-1)
		a(y, x+1) += (int)(7*e/16);
	    if((x != 0) && (y != a.rows-1))
		a(y+1, x-1) += (int)(3*e/16);
	    if(y != a.rows-1)
		a(y+1, x) += (int)(5*e/16);
	    if((x != a.cols-1) && (y != a.rows-1))
		a(y+1, x+1) += (int)(3*e/16);
	}
	cv::imshow("FloydSteinberg", result);
	cv::waitKey(20);
	if(!(y%(a.rows/3)))
	    capture(result, ".jpg");
    }
    cv::waitKey(0);
    return result;
}

int halfTonig()
{
	cv::Mat image = cv::imread("sample.jpg");	//画像の読み込み

	if (image.empty())	//画像が正しく読み込めたのかを確認
	{
		cout << "画像ファイルを読み込めませんでした．" << endl;
		return -1;
	}

	cv::Mat gray;
	cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

    try{
	CV_Assert(gray.data);
    }catch(cv::Exception e){
	return -1;
    }
    floydSteinberg(gray);
    return 0;
}

int main(int argc, char** argv)
{
	halfTonig();
	return 0;
}
