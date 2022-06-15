// 指定した任意四角形Aの領域を白に，その他を黒にした，入力画像と同じサイズのマスクを生成
// c: 置き換え実行
// r: やり直す
// s: ファイルに保存(argv[2]で指定)
#include <opencv2/opencv.hpp>

using namespace std;

cv::Mat input;	 //入力画像
cv::Mat imageLine; //枠線記憶用画像
cv::Mat imageTmp;  //マウス入力用表示画像
cv::Mat mask;	  //マスク画像

cv::Point startp;  //四角形の始点
cv::Point secondp; //四角形の第2点
cv::Point thirdp;  //四角形の第3点
cv::Point endp;	//四角形の終点
int rect = 0;	  //4点クリック完了フラグ

int p_count = 0;
cv::Scalar line_color = cv::Scalar(0, 250, 0); // 線の色 (default 緑)

void onFourMouseClicks(int event, int x, int y, int flag, void *param)
{
	using namespace cv;
	Mat *mat = static_cast<Mat *>(param);
	imageTmp = mat->clone();			//マウス移動中の表示用画像
	switch (event)
	{
	case cv::MouseEventTypes::EVENT_LBUTTONDOWN: //左ボタンが押されたら
		if (p_count == 0)
		{
			std::cout << "始点：(" << x << " , " << y << ")" << endl;
			line_color = cv::Scalar(0, 250, 0); // 線の色
			imageLine = input.clone();			//枠線表示用画像
			startp.x = x;
			startp.y = y;
			p_count = 1;
			break;
		}
		else if (p_count == 1)
		{
			std::cout << "第2点：(" << x << " , " << y << ")" << endl;
			secondp.x = x;
			secondp.y = y;
			line(imageLine, startp, secondp, line_color, 1); //1→2の直線描画
			p_count = 2;
			break;
		}
		else if (p_count == 2)
		{
			std::cout << "第3点：(" << x << " , " << y << ")" << endl;
			thirdp.x = x;
			thirdp.y = y;
			line(imageLine, secondp, thirdp, line_color, 1); //2→3直線描画
			p_count = 3;
			break;
		}
		else if (p_count == 3)
		{
			rect = 1; // 4点クリック完了
			std::cout << "終点：(" << x << " , " << y << ")" << endl;
			endp.x = x;
			endp.y = y;
			p_count = 0;
			line(imageLine, thirdp, endp, line_color, 1); //3→4直線描画
			line(imageLine, endp, startp, line_color, 1); //4→1直線描画
			imageTmp = imageLine.clone();
			break;
		}
	case cv::MouseEventTypes::EVENT_MOUSEMOVE:
		if (p_count == 1)
			line(imageTmp, startp, Point(x, y), line_color, 1); //1→mouse直線描画
		else if (p_count == 2)
			line(imageTmp, secondp, Point(x, y), line_color, 1); //2→mouse直線描画
		else if (p_count == 3)
			line(imageTmp, thirdp, Point(x, y), line_color, 1); //3→mouse直線描画
		break;
	}
}

// マウスクリックした4点を入力画像と同サイズの画像の4隅に射影変換する
void makeMask(char **args)
{
	input = cv::imread(args[1]);
	if (input.empty())
	{
		cout << "画像読み込み失敗" << endl;
		return;
	}
	//マウス入力用画像
	imageTmp = input.clone();
	imageLine = input.clone();
	//マスク画像の初期化
	mask = cv::Mat::zeros(input.size(), CV_8UC1);

	//ウィンドウ作成
	cv::namedWindow("mouse input window");

	cv::setMouseCallback("mouse input window", onFourMouseClicks, (void *)&imageLine);

	//画像の表示
	cv::imshow("mouse input window", imageTmp);

	//「q」が押されたら終了
	while (true)
	{
		//画像の表示
		cv::imshow("mouse input window", imageTmp);
		switch (cv::waitKey(33))
		{
		case 'c':
			if (rect == 1)
			{												  // 4点クリックが終わっていたならば
				mask = cv::Mat::zeros(input.size(), CV_8UC1); //マスク画像の0初期化
															  //				vector<vector<cv::Point>> contour; //輪郭情報はPointの配列(ベクタ)の配列(ベクタ)
				cv::drawContours(mask, vector<vector<cv::Point>>{{startp, secondp, thirdp, endp}}, 0, cv::Scalar::all(255), cv::FILLED);
				cv::imshow("Mask Image", mask);
				rect = 0;	// 4点クリックは最初から
				p_count = 0; // 4点クリックは最初から
			}
			break;
		case 's': // その時点の mask を保存
			cv::imwrite(args[2], mask);
			cout << "saved to " << args[2] << endl;
			break;
		case 'r': // 最初に戻る
			imageLine = input.clone();
			imageTmp = input.clone();
			rect = 0;	// 4点クリックは最初から
			p_count = 0; // 4点クリックは最初から
			cv::destroyWindow("Mask Image");
			cout << "Reset" << endl;
			break;
		case 'q':
			return;
		}
	}
}

int main(int argc, char *argv[])
{
	// makeMask関数に渡す配列の宣言
	char *args[3] = {argv[0], "../image/imageA.jpg", "../image/mask.jpg"};
	if (argc >= 2)
		args[1] = argv[1]; // 入力画像A
	if (argc >= 3)
		args[2] = argv[2]; // 入力画像B
	makeMask(args);
	return 0;
}
