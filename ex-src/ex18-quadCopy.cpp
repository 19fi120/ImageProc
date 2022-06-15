// 指定した任意四角形Aの領域を，指定した任意四角形Aの領域に射影変換
// c: 置き換え実行
// a: accumulate 蓄積（元画像に反映）
// s: ファイルに保存(argv[2]で指定)
// R: All Reset
// r: 最初の状態に戻る
// q: 辞める

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace cv;

cv::Mat input;	 //入力画像
cv::Mat imageLine; //枠線記憶用画像
cv::Mat imageTmp;  //マウス入力用表示画像
cv::Mat mask;	  //マスク画像
cv::Mat c;	//枠線
cv::Mat outImg;
cv::Mat pers_mat;  // 射影変換用3x3行列

cv::Point startp;		//矩形の始点
cv::Point secondp;		//矩形の第2点
cv::Point thirdp;		//矩形の第3点
cv::Point endp;			//矩形の終点

cv::Point startp_dash;		//2つ目の矩形の始点
cv::Point secondp_dash;		//2つ目矩形の第2点
cv::Point thirdp_dash;		//2つ目矩形の第3点
cv::Point endp_dash;	    //2つ目矩形の終点
int rect = 0;
int p_count = 0;
cv::Scalar line_color1 = cv::Scalar(0, 250, 0);  // 線の色
cv::Scalar line_color2 = cv::Scalar(0, 0, 250);  // 線の色

void onMouseClicks(int event, int x, int y, int flag, void* param) {
	using namespace cv;
	// Mat* mat = static_cast<Mat*>(param);	// Mat画像のポインタ受け取り
	// Mat c = mat->clone(); //元画像をクローン
	Mat *images = static_cast<Mat*>(param);	// Mat画像のポインタ受け取り
	Mat image = (*images).clone(); //元画像をクローン
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
		else if (p_count == 4){
			cout << "始点：(" << x << " , " << y << ")" << endl;
            startp_dash.x = x;
			startp_dash.y = y;
			p_count++;
			break;
		}
		else if (p_count == 5){
			cout << "第2点：(" << x << " , " << y << ")" << endl;
			secondp_dash.x = x;
			secondp_dash.y = y;
			p_count++;
			break;
		}
		else if (p_count == 6){
			cout << "第3点：(" << x << " , " << y << ")" << endl;
			thirdp_dash.x = x;
			thirdp_dash.y = y;
			p_count++;
			break;
		}
		else if (p_count == 7){
            //rect = 1; // 4点クリック完了
			cout << "終点：(" << x << " , " << y << ")" << endl;
			endp_dash.x = x;
			endp_dash.y = y;
			p_count++;
			break;
		}
		case cv::MouseEventTypes::EVENT_MOUSEMOVE:
		c.copyTo(imageLine);
		if (p_count == 1){
			line(imageLine, startp, Point(x, y), line_color1, 1); //1→mouse直線描画
            imshow("mouse input window", imageLine);
		}
		else if (p_count == 2){
			line(imageLine, startp, secondp, line_color1, 1); //1→mouse直線描画
			line(imageLine, secondp, Point(x, y), line_color1, 1); //2→mouse直線描画
            imshow("mouse input window", imageLine);
		}
		else if (p_count == 3){
			line(imageLine, startp, secondp, line_color1, 1); //1→mouse直線描画
			line(imageLine, secondp, thirdp, line_color1, 1); //2→mouse直線描画
			line(imageLine, thirdp, Point(x, y), line_color1, 1); //3→mouse直線描画
            imshow("mouse input window", imageLine);
		}
		else if (p_count == 5){
			line(imageLine, startp, secondp, line_color1, 1);
			line(imageLine, secondp, thirdp, line_color1, 1);
			line(imageLine, thirdp, endp, line_color1, 1);
			line(imageLine, endp, startp, line_color1, 1);

			line(imageLine, startp_dash, Point(x, y), line_color2, 1); //1→mouse直線描画
            imshow("mouse input window", imageLine);
		}
		else if (p_count == 6){
			line(imageLine, startp, secondp, line_color1, 1);
			line(imageLine, secondp, thirdp, line_color1, 1);
			line(imageLine, thirdp, endp, line_color1, 1);
			line(imageLine, endp, startp, line_color1, 1);

			line(imageLine, startp_dash, secondp_dash, line_color2, 1); //1→mouse直線描画
			line(imageLine, secondp_dash, Point(x, y), line_color2, 1); //2→mouse直線描画
            imshow("mouse input window", imageLine);
		}
		else if (p_count == 7){
			line(imageLine, startp, secondp, line_color1, 1);
			line(imageLine, secondp, thirdp, line_color1, 1);
			line(imageLine, thirdp, endp, line_color1, 1);
			line(imageLine, endp, startp, line_color1, 1);

			line(imageLine, startp_dash, secondp_dash, line_color2, 1); //1→mouse直線描画
			line(imageLine, secondp_dash, thirdp_dash, line_color2, 1); //2→mouse直線描画
			line(imageLine, thirdp_dash, Point(x, y), line_color2, 1); //3→mouse直線描画
            imshow("mouse input window", imageLine);
		}
		break;
		case cv::MouseEventTypes::EVENT_LBUTTONUP:
		if (p_count == 1){
			secondp.x = x;
			secondp.y = y;
			line(c, startp, secondp, line_color1, 1); //直線描画
			imshow("mouse input window", c);
		}
        else if (p_count == 2){
			thirdp.x = x;
			thirdp.y = y;
			line(c, startp, secondp, line_color1, 1); //直線描画
			line(c, secondp, thirdp, line_color1, 1); //直線描画
			imshow("mouse input window", c);
		}
        else if (p_count == 3){
			endp.x = x;
			endp.y = y;
			line(c, startp, secondp, line_color1, 1); //直線描画
			line(c, secondp, thirdp, line_color1, 1); //直線描画
			line(c, thirdp, endp, line_color1, 1); //直線描画
			imshow("mouse input window", c);
		}
        else if (p_count == 4){
			line(c, startp, secondp, line_color1, 1); //直線描画
			line(c, secondp, thirdp, line_color1, 1); //直線描画
			line(c, thirdp, endp, line_color1, 1); //直線描画
			line(c, endp, startp, line_color1, 1); //直線描画
			imshow("mouse input window", c);
		}
		else if (p_count == 5){
			line(c, startp, secondp, line_color1, 1); //直線描画
			line(c, secondp, thirdp, line_color1, 1); //直線描画
			line(c, thirdp, endp, line_color1, 1); //直線描画
			line(c, endp, startp, line_color1, 1); //直線描画

			secondp_dash.x = x;
			secondp_dash.y = y;
			line(c, startp_dash, secondp_dash, line_color2, 1); //直線描画
			imshow("mouse input window", c);
		}
        else if (p_count == 6){
			line(c, startp, secondp, line_color1, 1); //直線描画
			line(c, secondp, thirdp, line_color1, 1); //直線描画
			line(c, thirdp, endp, line_color1, 1); //直線描画
			line(c, endp, startp, line_color1, 1); //直線描画

			thirdp_dash.x = x;
			thirdp_dash.y = y;
			line(c, startp_dash, secondp_dash, line_color2, 1); //直線描画
			line(c, secondp_dash, thirdp_dash, line_color2, 1); //直線描画
			imshow("mouse input window", c);
		}
        else if (p_count == 7){
			line(c, startp, secondp, line_color1, 1); //直線描画
			line(c, secondp, thirdp, line_color1, 1); //直線描画
			line(c, thirdp, endp, line_color1, 1); //直線描画
			line(c, endp, startp, line_color1, 1); //直線描画

			endp_dash.x = x;
			endp_dash.y = y;
			line(c, startp_dash, secondp_dash, line_color2, 1); //直線描画
			line(c, secondp_dash, thirdp_dash, line_color2, 1); //直線描画
			line(c, thirdp_dash, endp_dash, line_color2, 1); //直線描画
			imshow("mouse input window", c);
		}
        else if (p_count == 8){
			line(c, startp, secondp, line_color1, 1); //直線描画
			line(c, secondp, thirdp, line_color1, 1); //直線描画
			line(c, thirdp, endp, line_color1, 1); //直線描画
			line(c, endp, startp, line_color1, 1); //直線描画
			
			line(c, startp_dash, secondp_dash, line_color2, 1); //直線描画
			line(c, secondp_dash, thirdp_dash, line_color2, 1); //直線描画
			line(c, thirdp_dash, endp_dash, line_color2, 1); //直線描画
			line(c, endp_dash, startp_dash, line_color2, 1); //直線描画
			imshow("mouse input window", c);
			p_count = 0;
			rect = 1;
		}
	}
}

// マウスクリックした4点を入力画像と同サイズの画像の4隅に射影変換する
void quadCopy(char **args){
	input = cv::imread(args[1]);
	if (input.empty()){
		cout << "画像読み込み失敗" << endl;
		return;
	}

	//マウス入力用画像
	c = input.clone();
	imageTmp = input.clone();
	imageLine = input.clone();
	//マスク画像の初期化
	mask = cv::Mat::zeros(input.size(), CV_8UC1);

	cv::Mat result;
	cv::Mat images[] = {
		input,
		c,
		imageTmp
	};
	//ウィンドウ作成
	cv::namedWindow("mouse input window");

	//マウスイベントが発生した時のコールバック関数を登録
	cv::setMouseCallback("mouse input window", onMouseClicks, (void*)&images);

	//画像の表示
	cv::imshow("mouse input window", imageLine);//インプットした画像の表示

	//「q」が押されたら終了
	while (true)
	{
		//画像の表示
		cv::imshow("mouse input window", imageLine);
		switch (cv::waitKey(33))
		{
			case 'c':
			if (rect == 1) // 4点クリックが終わっていたならば
			{
				//outImg = cv::Mat::zeros(c.size(), CV_8UC1);　//出力画像の0初期化
				outImg = cv::Mat::zeros(c.rows, c.cols, CV_8UC1);	//出力画像の0初期化
				// 射影変換
				// 左上、右上、右下、左下 が標準
				const Point2f src_pt[] = {
					Point2f(startp.x, startp.y),
					Point2f(secondp.x, secondp.y),
					Point2f(thirdp.x, thirdp.y),
					Point2f(endp.x, endp.y)
				};
				const Point2f dst_pt[] = {
					Point2f(startp_dash.x, startp_dash.y),
					Point2f(secondp_dash.x, secondp_dash.y),
					Point2f(thirdp_dash.x, thirdp_dash.y),
					Point2f(endp_dash.x, endp_dash.y)
				};
				pers_mat = getPerspectiveTransform(src_pt, dst_pt);
				// 射影変換行列pers_matにより，warpPerspectiveを用いて画像を変換
				warpPerspective(imageTmp, outImg, pers_mat, outImg.size());
				mask = cv::Mat::zeros(c.size(), CV_8UC1);
				cv::drawContours(mask, vector<vector<cv::Point>>{{startp_dash, secondp_dash, thirdp_dash, endp_dash}}, 0, cv::Scalar::all(255), cv::FILLED);//vector<vector<cv::Point>> contour;
																																							//輪郭情報contourはPointの配列(ベクタ)の配列(ベクタ)
																																							//4点 startp, secondp, thirdp, endp を直接指定し，
																																							//それらに囲まれる領域を(255,255,255)で塗り潰す例
				outImg.copyTo(imageTmp,mask);
                cv::imshow("Mask Image", mask);			//マスク画像の表示
                cv::imshow("Result", imageTmp);  //ブレンディング結果画像の表示
	            cv::waitKey();						    //入力があるまで待機
			}
			break;
			case 'a': //accumulate
	            imageTmp.copyTo(c);
				rect = 0;
				p_count = 0;
				cv::destroyWindow("Result");
	            cout << "Accumulate: " << endl;
	            break;
	        case 's': //save
	            cv::imwrite(args[2], imageTmp);
	            cout << "saved in: " << args[2] << endl;
	            break;
	        case 'R': //Reset
                input.copyTo(c);
				input.copyTo(imageTmp);
				input.copyTo(imageLine);
				rect = 0;
				p_count = 0;
				cv::destroyWindow("Result");
				cout << "All Reset" << endl;
	            break;
	        case 'r': //reset
				imageTmp.copyTo(imageLine);
				imageTmp.copyTo(c);
				rect = 0;
				p_count = 0;
				cv::destroyWindow("Result");
				cout << "Reset" << endl;
	            break;
	        case 'q':
	            return;
	    }
    }
}

int main(int argc, char* argv[])
{
	char *args[3] = {argv[0], "../image/imageA.jpg", "../image/result.jpg"};	//default画像
	if (argc >= 2)
		args[1] = argv[1]; // 画像
	if (argc >= 3)
		args[2] = argv[2]; // 出力画像

	quadCopy(args);
	return 0;
}
