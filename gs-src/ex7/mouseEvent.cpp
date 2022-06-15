#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace cv;

//===============================================
//マウスイベントのお試しプログラム.
//クリックした2点を始点・終点とした
//ROIの切り出し画像を表示する.
//===============================================

// マウスでクリックした2点をもとにROIを作成
cv::Point startp;	 //矩形の始点
cv::Point endp;		  //矩形の終点
bool roiflag = false; //矩形切り出しフラグ (クリック回数管理）
//マウスイベントで呼び出される関数
void onMouse(int event, int x, int y, int flag, void *param)
{
	switch (event)
	{
	case cv::MouseEventTypes::EVENT_LBUTTONDOWN: //左ボタンが押されたら
		//元画像のポインタを取得
		cv::Mat *matp = static_cast<cv::Mat *>(param);
		cv::Mat cImage = matp->clone(); // 入力画像のコピー
		if (!roiflag)
		{
			cout << "始点：(" << x << " , " << y << ")" << endl;
			startp.x = x;
			startp.y = y;
			cv::circle(cImage, cv::Point(startp.x, startp.y), 3, cv::Scalar(0, 0, 255), 2, cv::LineTypes::LINE_AA);
			//ウィンドウ「roi」を閉じる
			cv::destroyWindow("roi");
			cv::imshow("input", cImage);
			roiflag = true; //クリック1回目 false→true
		}
		else
		{
			cout << "終点：(" << x << " , " << y << ")" << endl;
			endp.x = x;
			endp.y = y;
			cv::rectangle(cImage, cv::Point(startp.x, startp.y), cv::Point(endp.x, endp.y), cv::Scalar(0, 0, 255), 2, cv::LineTypes::LINE_AA);
			cv::imshow("input", cImage);
			//ROIの生成
			if (startp.x - endp.x == 0 || startp.y - endp.y == 0)
				cout << "幅か高さが0となるROIは生成できません" << endl;
			else
			{
				cv::Mat roi(*matp, cv::Rect(startp, endp));
				cv::imshow("roi", roi);
			}
			roiflag = false; //クリック2回目 true→false
		}
	}
}

void mouseEvent(char *fileName)
{
	cv::Mat image = cv::imread(fileName);
	if (image.empty())
	{
		std::cout << "画像読み込み失敗" << std::endl;
		return;
	}

	//ウィンドウ作成
	cv::namedWindow("input");

	//マウスイベントが発生した時のコールバック関数を登録
	cv::setMouseCallback("input", onMouse, (void *)&image); //画像をコールバック関数に渡す

	//画像の表示
	cv::imshow("input", image);

	//「q」が押されたら終了
	while (cv::waitKey() != 'q')
		;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
		mouseEvent("sample.jpg"); //default画像
	else
		mouseEvent(argv[1]);

	return 0;
}