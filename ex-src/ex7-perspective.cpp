#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;

int key = -1;
cv::Point startp;		//矩形の始点
cv::Point secondp;		//矩形の第2点
cv::Point thirdp;		//矩形の第3点
cv::Point endp;			//矩形の終点
cv::Mat pers_mat;  // 射影変換用3x3行列
int p_count = 0;

void onFourMouseClicks(int event, int x, int y, int flag, void* param) {
	using namespace cv;
	Mat* mat = static_cast<Mat*>(param);	// Mat画像のポインタ受け取り
	Mat c = (*mat).clone(); //元画像をクローン
	//画像の表示
	switch (event)
	{
	case cv::MouseEventTypes::EVENT_LBUTTONDOWN:	//左ボタンが押されたら
		if (p_count == 0){
			cout << "始点：(" << x << " , " << y << ")" << endl;
			startp.x = x;
			startp.y = y;
			p_count++;
			break;
		}
		else if (p_count == 1){
			cout << "第2点：(" << x << " , " << y << ")" << endl;
			secondp.x = x;
			secondp.y = y;
			p_count++;
			break;
		}
		else if (p_count == 2){
			cout << "第3点：(" << x << " , " << y << ")" << endl;
			thirdp.x = x;
			thirdp.y = y;
			p_count++;
			break;
		}
		else if (p_count == 3){
			cout << "終点：(" << x << " , " << y << ")" << endl;
			endp.x = x;
			endp.y = y;
			p_count++;
			break;
		}

	case cv::MouseEventTypes::EVENT_MOUSEMOVE:
		if (p_count == 1){
			Mat* mat = static_cast<Mat*>(param);
			Mat c = (*mat).clone(); //元画像をクローン
			line(c, startp, Point(x, y), Scalar(0, 250, 0), 2); //直線描画
			imshow("mouse input window", c);
		}
		else if (p_count == 2){
			Mat* mat = static_cast<Mat*>(param);
			Mat c = (*mat).clone(); //元画像をクローン
			line(c, startp, secondp, Scalar(0, 250, 0), 2); //直線描画
			line(c, secondp, Point(x, y), Scalar(0, 250, 0), 2); //直線描画
			imshow("mouse input window", c);
		}
		else if (p_count == 3){
			Mat* mat = static_cast<Mat*>(param);
			Mat c = (*mat).clone(); //元画像をクローン
			line(c, startp, secondp, Scalar(0, 250, 0), 2); //直線描画
			line(c, secondp, thirdp, Scalar(0, 250, 0), 2); //直線描画
			line(c, thirdp, Point(x, y), Scalar(0, 250, 0), 2); //直線描画
			imshow("mouse input window", c);
		}
		break;

	case cv::MouseEventTypes::EVENT_LBUTTONUP:
		if (p_count == 1){
			Mat* mat = static_cast<Mat*>(param);
			Mat c = (*mat).clone(); //元画像をクローン
			secondp.x = x;
			secondp.y = y;
			line(c, startp, secondp, Scalar(0, 250, 0), 2); //直線描画			
			imshow("mouse input window", c);
		}
		else if (p_count == 2){
			Mat* mat = static_cast<Mat*>(param);
			Mat c = (*mat).clone(); //元画像をクローン
			thirdp.x = x;
			thirdp.y = y;
			line(c, startp, secondp, Scalar(0, 250, 0), 2); //直線描画
			line(c, secondp, thirdp, Scalar(0, 250, 0), 2); //直線描画
			imshow("mouse input window", c);
		}
		else if (p_count == 3){
			Mat* mat = static_cast<Mat*>(param);
			Mat c = (*mat).clone(); //元画像をクローン
			endp.x = x;
			endp.y = y;
			line(c, startp, secondp, Scalar(0, 250, 0), 2); //直線描画
			line(c, secondp, thirdp, Scalar(0, 250, 0), 2); //直線描画
			line(c, thirdp, endp, Scalar(0, 250, 0), 2); //直線描画
			imshow("mouse input window", c);
		}
		else if (p_count == 4){
			Mat* mat = static_cast<Mat*>(param);
			Mat c = (*mat).clone(); //元画像をクローン
			line(c, startp, secondp, Scalar(0, 250, 0), 2); //直線描画
			line(c, secondp, thirdp, Scalar(0, 250, 0), 2); //直線描画
			line(c, thirdp, endp, Scalar(0, 250, 0), 2); //直線描画
			line(c, endp, startp, Scalar(0, 250, 0), 2); //直線描画
			imshow("mouse input window", c);
			p_count = 0;
			// 射影変換
			Mat outImg = Mat::zeros(c.rows, c.cols, CV_8UC3);
			// 左上、右上、右下、左下 が標準
			const Point2f src_pt[] = {
				Point2f(startp.x, startp.y),
				Point2f(secondp.x, secondp.y),
				Point2f(thirdp.x, thirdp.y),
				Point2f(endp.x, endp.y)
			};
			const Point2f dst_pt[] = {
				Point2f(0.0, 0.0),
				Point2f(c.cols, 0.0),
				Point2f(c.cols, c.rows),
				Point2f(0.0, c.rows)
			};
			pers_mat = getPerspectiveTransform(src_pt, dst_pt);
			// 射影変換行列pers_matにより，warpPerspectiveを用いて画像を変換
			warpPerspective(c, outImg, pers_mat, outImg.size());
			imshow("Perspective Image", outImg);
		}
	}
}

// マウスクリックした4点を入力画像と同サイズの画像の4隅に射影変換する
void perspective(char* fileName){
	cv::Mat image = cv::imread(fileName);
	if (image.empty()){
		cout << "画像読み込み失敗" << endl;
		return;
	}
	//ウィンドウ作成
	cv::namedWindow("mouse input window");

	//マウスイベントが発生した時のコールバック関数を登録
	cv::setMouseCallback("mouse input window", onFourMouseClicks, (void*)&image);

	//画像の表示
	cv::imshow("mouse input window", image);

	//「q」が押されたら終了
	while (cv::waitKey() != 'q');
}

int main(int argc, char* argv[])
{
	if (argc < 2) perspective("sample.jpg");	//default画像
	else perspective(argv[1]);
	return 0;
}
