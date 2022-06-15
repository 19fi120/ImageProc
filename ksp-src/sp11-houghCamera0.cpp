#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;

void cannyAndHough(cv::Mat src, cv::Mat dst, double rho, 
		double theta, int threshold, double minL, double maxLineGap)
	{
	using namespace cv;

	Mat edges;
	// Threshold low = 100, Threshold hight = 200, Line thickness = 3
    Canny( src, edges, 100, 200, 3 );
    cvtColor( edges, dst, COLOR_GRAY2BGR );
    vector<Vec4i> lines;

	// edges: 入力画像（2値画像）
	// lines: 出力される線の配列
	// rho: 原点から同じ距離と見なす幅（距離解像度）
	// theta: 同じ角度と見なす角度の幅（角度解像度）
	// threshold: 投票数の閾値
	// minL: 検出すべき最小長 （省略可能）
	// maxLineGap: 同じ線上の画素間の最大ギャップ（省略可能）
    HoughLinesP( edges, lines, rho, theta, threshold, minL, maxLineGap);
    for( size_t i = 0; i < lines.size(); i++ ) {
        line( dst, Point(lines[i][0], lines[i][1]),
		// Scalar(0,0,255): Red line, Line thickness = 2, Line type = 8-connection
        Point( lines[i][2], lines[i][3]), Scalar(0,0,255), 2, 8 );
    }
}

int main(int argc, char* argv[])
{
	cv::Mat frame;
	cv::VideoCapture cap(0);			//カメラ入力の読み込み
	if (cap.isOpened() == false)	//カメラ入力の読み込みに失敗したら終了
	{
		cout << "カメラ入力失敗" << endl;
		return -1;
	}
	cap >> frame;	//カメラからの画像を取得

    cout << "q を押すと終了" << endl;
    cv::Mat dst = frame.clone();
	int threshold = 100;
	double minL = 50;

	//ループ開始, q ならループ終了
	while (true)
	{
		cap >> frame;	//カメラからの画像を取得
		if (frame.empty())	//画像が正しく読み込めたのかを確認
		{
			cout << "カメラ入力のために待機" << endl;
			continue;
		}
		cv::imshow("Input", frame);				//入力画像の表示
		cannyAndHough(frame, dst, 1, CV_PI/180, threshold, minL, 10);
		cv::imshow("Hough Transform", dst);		//ハフ変換結果の表示
        if (cv::waitKey(33)=='q') return 0;
	}
}
