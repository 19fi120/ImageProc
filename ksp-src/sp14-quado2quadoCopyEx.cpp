#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;

cv::Mat input;			//入力画像
cv::Mat imagePrev;		//1ステップ前の処理結果画像
cv::Mat image;			//射影変換前画像
cv::Mat imageLine;		//枠線表示用画像
cv::Mat c;				//マウス移動中の表示用画像
cv::Mat outImg;			//射影変換結果画像
int key = -1;
cv::Point startp, src_sp, dst_sp;		//四角形の始点
cv::Point secondp, src_2p, dst_2p;		//四角形の第2点
cv::Point thirdp, src_3p, dst_3p;		//四角形の第3点
cv::Point endp, src_ep, dst_ep;			//四角形の終点

cv::Mat pers_mat;  // 射影変換用3x3行列
int p_count = 0;
int rect_count = 0;		// 0: コピー元の矩形，1: コピー先の矩形，2: 準備完了
cv::Scalar line_color = cv::Scalar(0, 250, 0);  // 線の色 (default 緑)

void onFourMouseClicks(int event, int x, int y, int flag, void* param) {
	using namespace cv;
	switch (event)
	{
	case cv::MouseEventTypes::EVENT_LBUTTONDOWN:	//左ボタンが押されたら
		if (rect_count == 2){
			rect_count = 0;
		}
		if (p_count == 0){
			cout << "始点：(" << x << " , " << y << ")" << endl;
			if (rect_count == 0){
				line_color = cv::Scalar(0, 250, 0);  // 線の色
				Mat* mat = static_cast<Mat*>(param);
				c = (*mat).clone(); //元画像をクローン
				imagePrev = image.clone();
				imageLine = image.clone();
			} else {
				line_color = cv::Scalar(0, 0, 250);  // 線の色
			}
			startp.x = x;
			startp.y = y;
			p_count = 1;
			break;
		}
		else if (p_count == 1){
			cout << "第2点：(" << x << " , " << y << ")" << endl;
			secondp.x = x;
			secondp.y = y;
			line(imageLine, startp, secondp, line_color, 1); //直線描画
			p_count = 2;
			break;
		}
		else if (p_count == 2){
			cout << "第3点：(" << x << " , " << y << ")" << endl;
			thirdp.x = x;
			thirdp.y = y;
			line(imageLine, secondp, thirdp, line_color, 1); //直線描画
			p_count = 3;
			break;
		}
		else if (p_count == 3){
			cout << "終点：(" << x << " , " << y << ")" << endl;
			endp.x = x;
			endp.y = y;
			p_count = 0;
			line(imageLine, thirdp, endp, line_color, 1); //直線描画
			line(imageLine, endp, startp, line_color, 1); //直線描画
			cout << "current rect_count = " << rect_count << endl;
			if (rect_count == 0){
				src_sp = startp;
				src_2p = secondp;
				src_3p = thirdp;
				src_ep = endp;
				rect_count = 1;
				cout << "rect_count changes 0 to 1" << endl;
			}
			else if (rect_count == 1){
				dst_sp = startp;
				dst_2p = secondp;
				dst_3p = thirdp;
				dst_ep = endp;
				rect_count = 2;
				cout << "rect_count changes 1 to 2" << endl;
			};
			break;
		}

	case cv::MouseEventTypes::EVENT_MOUSEMOVE:
		if (p_count == 1){
			Mat* mat = static_cast<Mat*>(param);
			c = (*mat).clone(); //元画像をクローン
			line(c, startp, Point(x, y), line_color, 1); //直線描画
		}
		else if (p_count == 2){
			Mat* mat = static_cast<Mat*>(param);
			c = (*mat).clone(); //元画像をクローン
			line(c, secondp, Point(x, y), line_color, 1); //直線描画
		}
		else if (p_count == 3){
			Mat* mat = static_cast<Mat*>(param);
			c = (*mat).clone(); //元画像をクローン
			line(c, thirdp, Point(x, y), line_color, 1); //直線描画
		}
		break;
	}
}

// マウスクリックした4点を入力画像と同サイズの画像の4隅に射影変換する
void perspective(char* fileName, char* writeFileName){
	input = cv::imread(fileName);
	if (input.empty()){
		cout << "画像読み込み失敗" << endl;
		return;
	}
	//マウス入力用画像
	imagePrev = input.clone();	// 1ステップ前の結果保存用
	imageLine = input.clone();
	image = input.clone();
	c = image.clone();
	//ウィンドウ作成
	cv::namedWindow("mouse input window");

	cv::setMouseCallback("mouse input window", onFourMouseClicks, (void*)&imageLine);

	//画像の表示
	cv::imshow("mouse input window", image);

	//「q」が押されたら終了
	while (true){
		c = imageLine.clone();
		if (rect_count == 3){
			p_count = 0;
			rect_count = 0;
			cout << "rect_count = 0 reset" << endl;
		} 
	    switch (cv::waitKey(10)){
            case 'c':
			if (rect_count == 2){
				outImg = cv::Mat::zeros(image.rows, image.cols, CV_8UC3);
				const cv::Point2f src_pt[] = {
					cv::Point2f(src_sp.x, src_sp.y),
					cv::Point2f(src_2p.x, src_2p.y),
					cv::Point2f(src_3p.x, src_3p.y),
					cv::Point2f(src_ep.x, src_ep.y)
				};
				const cv::Point2f dst_pt[] = {
					cv::Point2f(dst_sp.x, dst_sp.y),
					cv::Point2f(dst_2p.x, dst_2p.y),
					cv::Point2f(dst_3p.x, dst_3p.y),
					cv::Point2f(dst_ep.x, dst_ep.y)
				};
				pers_mat = getPerspectiveTransform(src_pt, dst_pt);
				// 射影変換行列pers_matにより，warpPerspectiveを用いて画像を変換
				warpPerspective(image, outImg, pers_mat, outImg.size());

				cv::Mat mask = cv::Mat::zeros(outImg.size(),CV_8UC1);
				vector<vector<cv::Point>> contour;
				vector<cv::Point> pts;
				pts.push_back(dst_sp);
				pts.push_back(dst_2p);
				pts.push_back(dst_3p);
				pts.push_back(dst_ep);
				contour.push_back(pts);
				cv::drawContours(mask,contour,0,cv::Scalar::all(255),-1);
				outImg.copyTo(image, mask);
				cv::imshow("Perspective Image", outImg);
				cv::imshow("Result Image", image);
				}
                break;
            case 'C':
			if (rect_count == 2){
				outImg = cv::Mat::zeros(image.rows, image.cols, CV_8UC3);

 				dst_2p.x = dst_sp.x + (src_2p.x - src_sp.x);
 				dst_3p.x = dst_ep.x + (src_3p.x - src_ep.x);
				const cv::Point2f src_pt[] = {
					cv::Point2f(src_sp.x, src_sp.y),
					cv::Point2f(src_2p.x, src_2p.y),
					cv::Point2f(src_3p.x, src_3p.y),
					cv::Point2f(src_ep.x, src_ep.y)
				};
				const cv::Point2f dst_pt[] = {
					cv::Point2f(dst_sp.x, dst_sp.y),
					cv::Point2f(dst_2p.x, dst_2p.y),
					cv::Point2f(dst_3p.x, dst_3p.y),
					cv::Point2f(dst_ep.x, dst_ep.y)
				};
				pers_mat = getPerspectiveTransform(src_pt, dst_pt);
				// 射影変換行列pers_matにより，warpPerspectiveを用いて画像を変換
				warpPerspective(image, outImg, pers_mat, outImg.size());
				vector<vector<cv::Point>> contour;
				vector<cv::Point> pts;
				cv::Mat mask = cv::Mat::zeros(outImg.size(),CV_8UC1);

				pts.push_back(dst_sp);
				pts.push_back(dst_2p);
				pts.push_back(dst_3p);
				pts.push_back(dst_ep);
				contour.push_back(pts);
				cv::drawContours(mask,contour,0,cv::Scalar::all(255),-1);
				outImg.copyTo(image, mask);

 				dst_sp.x = dst_sp.x + (src_2p.x - src_sp.x);
 				dst_ep.x = dst_ep.x + (src_3p.x - src_ep.x);
 				dst_2p.x = dst_sp.x + (src_2p.x - src_sp.x);
 				dst_3p.x = dst_ep.x + (src_3p.x - src_ep.x);
				const cv::Point2f src_pt1[] = {
					cv::Point2f(src_sp.x, src_sp.y),
					cv::Point2f(src_2p.x, src_2p.y),
					cv::Point2f(src_3p.x, src_3p.y),
					cv::Point2f(src_ep.x, src_ep.y)
				};
				const cv::Point2f dst_pt1[] = {
					cv::Point2f(dst_sp.x, dst_sp.y),
					cv::Point2f(dst_2p.x, dst_2p.y),
					cv::Point2f(dst_3p.x, dst_3p.y),
					cv::Point2f(dst_ep.x, dst_ep.y)
				};
				pers_mat = getPerspectiveTransform(src_pt1, dst_pt1);
				// 射影変換行列pers_matにより，warpPerspectiveを用いて画像を変換
				warpPerspective(image, outImg, pers_mat, outImg.size());

				mask = cv::Mat::zeros(outImg.size(),CV_8UC1);
				pts.push_back(dst_sp);
				pts.push_back(dst_2p);
				pts.push_back(dst_3p);
				pts.push_back(dst_ep);
				contour.push_back(pts);
				cv::drawContours(mask,contour,0,cv::Scalar::all(255),-1);
				outImg.copyTo(image, mask);

				cv::imshow("Perspective Image", outImg);
				cv::imshow("Result Image", image);
				}
                break;
            case 'a':		// 結果蓄積
				rect_count = 3;
				imagePrev = image.clone();
				c = image.clone();
				imageLine = image.clone();
				cv::destroyWindow("Result Image");
				cv::destroyWindow("Perspective Image");
                break;
            case 's':
				cv::imwrite(writeFileName, image);
				cout << "saved to " << writeFileName << endl;
                break;
            case 'r':		// 1ステップ前の結果に戻る
				rect_count = 3;
				image = imagePrev.clone();
				imageLine = imagePrev.clone();
				cout << "redo" << endl;
				cv::destroyWindow("Perspective Image");
				cv::destroyWindow("Result Image");
                break;
            case 'R':		// 最初に戻る
				rect_count = 3;
				image = input.clone();
				imagePrev = input.clone();
				imageLine = input.clone();
				cv::destroyWindow("Perspective Image");
				cv::destroyWindow("Result Image");
				cout << "Reset" << endl;
                break;
            case 'q':
                return;
        }
	//画像の表示
	cv::imshow("mouse input window", c);
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2) perspective("sample.jpg", "sampleResult.jpg");	//default画像
	else if (argc < 3) perspective(argv[1], "sampleResult.jpg");
	else perspective(argv[1], argv[2]);
	return 0;
}
