#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;

cv::Mat input;	 //入力画像
cv::Mat imageLine; //枠線記憶用画像
cv::Mat imageTmp;  //マウス入力用表示画像
cv::Mat mask;	  //マスク画像

cv::Point startp;  //四角形の始点
cv::Point secondp; //四角形の第2点
cv::Point thirdp;  //四角形の第3点
cv::Point endp;	//四角形の終点
cv::Mat pers_mat;  // 射影変換用3x3行列

int rect = 0;	  //4点クリック完了フラグ
int p_count = 0;
cv::Scalar line_color = cv::Scalar(0, 250, 0); // 線の色 (default 緑)

void onFourMouseClicks(int event, int x, int y, int flag, void* param)
{
	using namespace cv;
	Mat* mat = static_cast<Mat*>(param);	// Mat画像のポインタ受け取り
	Mat imageTmp = mat->clone();			//元画像をクローン
	Mat imageA = mat[0];
	Mat imageB = mat[1];
	Mat c = imageA.clone();//元画像をクローン
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
            rect = 1; // 4点クリック完了
			cout << "終点：(" << x << " , " << y << ")" << endl;
			endp.x = x;
			endp.y = y;
			p_count++;
			break;
		}
	case cv::MouseEventTypes::EVENT_MOUSEMOVE:
		if (p_count == 1){
			line(c, startp, Point(x, y), line_color, 1); //1→mouse直線描画
            imshow("mouse input window", c);
		}
		else if (p_count == 2){
			line(c, startp, secondp, line_color, 1); //1→mouse直線描画
			line(c, secondp, Point(x, y), line_color, 1); //2→mouse直線描画
            imshow("mouse input window", c);
		}
		else if (p_count == 3){
			line(c, startp, secondp, line_color, 1); //1→mouse直線描画
			line(c, secondp, thirdp, line_color, 1); //2→mouse直線描画
			line(c, thirdp, Point(x, y), line_color, 1); //3→mouse直線描画
            imshow("mouse input window", c);
		}
		break;

	case cv::MouseEventTypes::EVENT_LBUTTONUP:
        if (p_count == 1){
			secondp.x = x;
			secondp.y = y;
			line(c, startp, secondp, line_color, 1); //直線描画
			imshow("mouse input window", c);
		}
        else if (p_count == 2){
			thirdp.x = x;
			thirdp.y = y;
			line(c, startp, secondp, line_color, 1); //直線描画
			line(c, secondp, thirdp, line_color, 1); //直線描画
			imshow("mouse input window", c);
		}
        else if (p_count == 3){
			endp.x = x;
			endp.y = y;
			line(c, startp, secondp, line_color, 1); //直線描画
			line(c, secondp, thirdp, line_color, 1); //直線描画
			line(c, thirdp, endp, line_color, 1); //直線描画
			imshow("mouse input window", c);
		}
        else if (p_count == 4){
			line(c, startp, secondp, line_color, 1); //直線描画
			line(c, secondp, thirdp, line_color, 1); //直線描画
			line(c, thirdp, endp, line_color, 1); //直線描画
			line(c, endp, startp, line_color, 1); //直線描画
			imshow("mouse input window", c);
			p_count = 0;
			// 射影変換
			//Mat outImg = Mat::zeros(imageTmp.rows, imageTmp.cols, CV_8UC3);
			// 左上、右上、右下、左下 が標準
			const Point2f src_pt[] = {
				Point2f(startp.x, startp.y),
				Point2f(secondp.x, secondp.y),
				Point2f(thirdp.x, thirdp.y),
				Point2f(endp.x, endp.y)
			};
			const Point2f dst_pt[] = {
				Point2f(0.0, 0.0),
				Point2f(imageB.cols, 0.0),
				Point2f(imageB.cols, imageB.rows),
				Point2f(0.0, imageB.rows)
			};
			pers_mat = getPerspectiveTransform(dst_pt, src_pt);
			// 射影変換行列pers_matにより，warpPerspectiveを用いて画像を変換
			Mat outImg = imageA.clone();
			Mat imageBperspective = imageB.clone();
			warpPerspective(imageB, imageBperspective, pers_mat, c.size());
			Mat mask = cv::Mat::zeros(c.size(), CV_8UC1);
			drawContours(mask, vector<vector<cv::Point>>{{startp, secondp, thirdp, endp}}, 0, cv::Scalar::all(255), cv::FILLED);
			imageBperspective.copyTo(outImg,mask);
			imshow("Mask Image", mask);
			imshow("Perspective Image", outImg);
		}
	}
}

// マウスクリックした4点に画像を射影変換する
void perspective2(char **args){
	cv:: Mat input = cv::imread(args[1]);
	if (input.empty()){
		cout << "画像読み込み失敗" << endl;
		return;
	}
    cv:: Mat imageB = cv::imread(args[2]);
	if (imageB.empty())
	{
		cout << "画像B読み込み失敗" << endl;
		return;
	}

	//マウス入力用画像
	imageTmp = input.clone();
	imageLine = input.clone();
	//マスク画像の初期化
	mask = cv::Mat::zeros(input.size(), CV_8UC1);

	//ウィンドウ作成
	cv::namedWindow("mouse input window");

	//マウスイベントが発生した時のコールバック関数を登録
	cv::setMouseCallback("mouse input window", onFourMouseClicks, (void*)&imageLine);

	//画像の表示
	cv::imshow("mouse input window", input);//インプットした画像の表示
	cv::imshow("mouseB", imageB);//インプットした画像の表示

	//「q」が押されたら終了
	while (cv::waitKey() != 'q');
}

int main(int argc, char* argv[])
{
    char *args[3] = {argv[0], "../image/imageA.jpg", "../image/imageB.jpg"};	//default画像
	if (argc >= 2)
		args[1] = argv[1]; // 画像A
	if (argc >= 3)
		args[2] = argv[2]; // 画像B

	perspective2(args);
	return 0;
}
