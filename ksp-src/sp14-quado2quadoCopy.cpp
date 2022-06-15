// 指定した任意四角形Aの領域を別に任意四角形Bに転写する
// c: A を B に射影変換
//     (Aの4点指定がBと異なる順序の場合，点の対応を取って転写)
// a: 転写結果を蓄積
// s: ファイルに保存(argv[2]で指定)
// r: 1つ前の転写結果に戻す
// R: 入力画像に戻す
// h: if(Bの幅＞Aの幅) Bの中をAの幅で繰り返して転写する
//     (Aの4点指定がBと異なる順序の場合，点の対応を取って転写)
// v: if(Bの高さ＞Aの高さ) Bの中をAの高さで繰り返して転写する
//     (Aの4点指定がBと異なる順序の場合，点の対応を取って転写)
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
// x: Aの指定の第1点と第2点を基準線Laとし，基準線Laまでの第3点および第4点からの距離を
//    刻み幅Wとして，Bの第1点と第2点を結ぶ基準線Lbからの距離をWごとに区切って転写する
//  2点(x1,y1),(x2,y2)を通る直線の式： (x2−x1)(y−y1)=(y2−y1)(x−x1)
//									 ∴ a*x + b*y + c = 
//										(y2-y1)*x + (x1-x2)*y + (x2-x1)*y1 + (y1-y2)*x1 
//  直線 ax + by + c = 0 と点(x0,y0)の距離d： d=|a*x0+b*y0+c|/sqrt(a^2+b^2)
            case 'x':
			if (rect_count == 2){
				vector<vector<cv::Point>> contour(1);
				vector<cv::Point> pts(4);
				cv::Mat mask = cv::Mat::zeros(outImg.size(),CV_8UC1);
				int dst_3p0x = dst_3p.x;
				int dst_3p0y = dst_3p.y;
				int dst_ep0x = dst_ep.x;
				int dst_ep0y = dst_ep.y;
				float a, b, c;
				int x1, y1, x2, y2;
				x1 = src_sp.x;
				y1 = src_sp.y;
				x2 = src_2p.x;
				y2 = src_2p.y;
				// a*x + b*y + c = (y2-y1)*x + (x1-x2)*y + (x2-x1)*y1 + (y1-y2)*x1 
				a = (float)(y2-y1);
				b = (float)(x1-x2);
				c = (float)((x2-x1)*y1 + (y1-y2)*x1);
				//  基準線(第1点から第2点) と 第3点(3p)，第4点(ep)の距離
				const float	src_d3 = (float)abs(a*src_3p.x + b*src_3p.y + c)/sqrt(a*a+b*b);
				const float src_de = (float)abs(a*src_ep.x + b*src_ep.y + c)/sqrt(a*a+b*b);
				const cv::Point2f src_pt[] = {
					cv::Point2f(src_sp.x, src_sp.y),
					cv::Point2f(src_2p.x, src_2p.y),
					cv::Point2f(src_3p.x, src_3p.y),
					cv::Point2f(src_ep.x, src_ep.y)
				};
				cv::Point2f dst_pt[] = {
					cv::Point2f(0, 0),
					cv::Point2f(0, 0),
					cv::Point2f(0, 0),
					cv::Point2f(0, 0)
				};
				// dstの基準線 と 第3点(3p)，第4点(ep)の距離 dst_d3, dst_de が
				// srcの基準線 と 第3点(3p)，第4点(ep)の距離 src_d3, src_de よりも
				// それぞれ大きい場合は dstの基準線，第2点から最終第3点に向けて src_d3/dst_d3
				// 離れた第3点，第1点から最終第4点に向けて src_de/dst_de 離れた第4点を転写先とする
				x1 = dst_sp.x;
				x2 = dst_2p.x;
				y1 = dst_sp.y;
				y2 = dst_2p.y;
				// a*x + b*y + c = (y2-y1)*x + (x1-x2)*y + (x2-x1)*y1 + (y1-y2)*x1 
				a = (float)(y2-y1);
				b = (float)(x1-x2);
				c = (float)((x2-x1)*y1 + (y1-y2)*x1);
				float dst_d3, dst_de;
				//  基準線(第1点から第2点) と 第3点(3p)，第4点(ep)の距離
				dst_d3 = (float)abs(a*dst_3p0x + b*dst_3p0y + c)/sqrt(a*a+b*b);
				dst_de = (float)abs(a*dst_ep0x + b*dst_ep0y + c)/sqrt(a*a+b*b);
				dst_ep = dst_sp;	// 1回目のため
				dst_3p = dst_2p;	// 1回目のため
				while(dst_d3 > src_d3 && dst_de > src_de){
					dst_sp = dst_ep;			// 2回目以降のため
					dst_2p = dst_3p;			// 2回目以降のため
 					dst_3p.x = dst_2p.x + (int)(dst_3p0x - dst_2p.x) * src_d3 / dst_d3;
 					dst_3p.y = dst_2p.y + (int)(dst_3p0y - dst_2p.y) * src_d3 / dst_d3;
 					dst_ep.x = dst_sp.x + (int)(dst_ep0x - dst_sp.x) * src_de / dst_de;
 					dst_ep.y = dst_sp.y + (int)(dst_ep0y - dst_sp.y) * src_de / dst_de;
// 確認用		cout << "dst_3p, dst_ep = " << dst_3p << ", " << dst_ep << endl;
 					dst_pt[0] = cv::Point2f(dst_sp.x, dst_sp.y);
					dst_pt[1] = cv::Point2f(dst_2p.x, dst_2p.y);
					dst_pt[2] = cv::Point2f(dst_3p.x, dst_3p.y);
					dst_pt[3] = cv::Point2f(dst_ep.x, dst_ep.y);
					pers_mat = getPerspectiveTransform(src_pt, dst_pt);
					// 射影変換行列pers_matにより，warpPerspectiveを用いて画像を変換
					warpPerspective(image, outImg, pers_mat, outImg.size());
					mask = cv::Mat::zeros(outImg.size(),CV_8UC1);
					pts[0] = dst_sp;
					pts[1] = dst_2p;
					pts[2] = dst_3p;
					pts[3] = dst_ep;
					contour[0] = pts;
//	確認用描画	cv::circle(image, cv::Point(dst_3p.x, dst_3p.y), 2, cv::Scalar(0, 0, 255), 2, CV_AA);
//	確認用描画	cv::circle(image, cv::Point(dst_ep.x, dst_ep.y), 2, cv::Scalar(0, 255, 0), 2, CV_AA);
					cv::drawContours(mask,contour,0,cv::Scalar::all(255),-1);
//確認用最終マスク画像
					cv::imshow("Last mask", mask);
					outImg.copyTo(image, mask);
					x1 = dst_ep.x;
					x2 = dst_3p.x;
					y1 = dst_ep.y;
					y2 = dst_3p.y;
				// a*x + b*y + c = (y2-y1)*x + (x1-x2)*y + (x2-x1)*y1 + (y1-y2)*x1 
					a = (float)(y2-y1);
					b = (float)(x1-x2);
					c = (float)((x2-x1)*y1 + (y1-y2)*x1);
					dst_d3 = (float)abs(a*dst_3p0x + b*dst_3p0y + c)/sqrt(a*a+b*b);
					dst_de = (float)abs(a*dst_ep0x + b*dst_ep0y + c)/sqrt(a*a+b*b);
				}
				cv::imshow("Result Image", image);
			}
                break;
// X: Aの指定の第1点と第2点を基準線Laとし，基準線Laまでの第3点および第4点からの距離を
//    刻み幅Wとして，Bの第1点と第2点を結ぶ基準線Lbからの距離をWごとに区切って転写する
//    ★★ 転写の際，転写部分の境界をぼかす ★★
//  2点(x1,y1),(x2,y2)を通る直線の式： (x2−x1)(y−y1)=(y2−y1)(x−x1)
//									 ∴ a*x + b*y + c = 
//										(y2-y1)*x + (x1-x2)*y + (x2-x1)*y1 + (y1-y2)*x1 
//  直線 ax + by + c = 0 と点(x0,y0)の距離d： d=|a*x0+b*y0+c|/sqrt(a^2+b^2)
            case 'X':
			if (rect_count == 2){
				vector<vector<cv::Point>> contour(1);
				vector<cv::Point> pts(4);
				cv::Mat mask = cv::Mat::zeros(outImg.size(),CV_8UC1);
				int dst_3p0x = dst_3p.x;
				int dst_3p0y = dst_3p.y;
				int dst_ep0x = dst_ep.x;
				int dst_ep0y = dst_ep.y;
				float a, b, c;
				int x1, y1, x2, y2;
				x1 = src_sp.x;
				y1 = src_sp.y;
				x2 = src_2p.x;
				y2 = src_2p.y;
				// a*x + b*y + c = (y2-y1)*x + (x1-x2)*y + (x2-x1)*y1 + (y1-y2)*x1 
				a = (float)(y2-y1);
				b = (float)(x1-x2);
				c = (float)((x2-x1)*y1 + (y1-y2)*x1);
				//  基準線(第1点から第2点) と 第3点(3p)，第4点(ep)の距離
				const float	src_d3 = (float)abs(a*src_3p.x + b*src_3p.y + c)/sqrt(a*a+b*b);
				const float src_de = (float)abs(a*src_ep.x + b*src_ep.y + c)/sqrt(a*a+b*b);
				const cv::Point2f src_pt[] = {
					cv::Point2f(src_sp.x, src_sp.y),
					cv::Point2f(src_2p.x, src_2p.y),
					cv::Point2f(src_3p.x, src_3p.y),
					cv::Point2f(src_ep.x, src_ep.y)
				};
				cv::Point2f dst_pt[] = {
					cv::Point2f(0, 0),
					cv::Point2f(0, 0),
					cv::Point2f(0, 0),
					cv::Point2f(0, 0)
				};
				// dstの基準線 と 第3点(3p)，第4点(ep)の距離 dst_d3, dst_de が
				// srcの基準線 と 第3点(3p)，第4点(ep)の距離 src_d3, src_de よりも
				// それぞれ大きい場合は dstの基準線，第2点から最終第3点に向けて src_d3/dst_d3
				// 離れた第3点，第1点から最終第4点に向けて src_de/dst_de 離れた第4点を転写先とする
				x1 = dst_sp.x;
				x2 = dst_2p.x;
				y1 = dst_sp.y;
				y2 = dst_2p.y;
				// a*x + b*y + c = (y2-y1)*x + (x1-x2)*y + (x2-x1)*y1 + (y1-y2)*x1 
				a = (float)(y2-y1);
				b = (float)(x1-x2);
				c = (float)((x2-x1)*y1 + (y1-y2)*x1);
				float dst_d3, dst_de;
				//  基準線(第1点から第2点) と 第3点(3p)，第4点(ep)の距離
				dst_d3 = (float)abs(a*dst_3p0x + b*dst_3p0y + c)/sqrt(a*a+b*b);
				dst_de = (float)abs(a*dst_ep0x + b*dst_ep0y + c)/sqrt(a*a+b*b);
				dst_ep = dst_sp;	// 1回目のため
				dst_3p = dst_2p;	// 1回目のため
				while(dst_d3 > src_d3 && dst_de > src_de){
					dst_sp = dst_ep;			// 2回目以降のため
					dst_2p = dst_3p;			// 2回目以降のため
 					dst_3p.x = dst_2p.x + (int)(dst_3p0x - dst_2p.x) * src_d3 / dst_d3;
 					dst_3p.y = dst_2p.y + (int)(dst_3p0y - dst_2p.y) * src_d3 / dst_d3;
 					dst_ep.x = dst_sp.x + (int)(dst_ep0x - dst_sp.x) * src_de / dst_de;
 					dst_ep.y = dst_sp.y + (int)(dst_ep0y - dst_sp.y) * src_de / dst_de;
// 確認用		cout << "dst_3p, dst_ep = " << dst_3p << ", " << dst_ep << endl;
 					dst_pt[0] = cv::Point2f(dst_sp.x, dst_sp.y);
					dst_pt[1] = cv::Point2f(dst_2p.x, dst_2p.y);
					dst_pt[2] = cv::Point2f(dst_3p.x, dst_3p.y);
					dst_pt[3] = cv::Point2f(dst_ep.x, dst_ep.y);
					pers_mat = getPerspectiveTransform(src_pt, dst_pt);
					// 射影変換行列pers_matにより，warpPerspectiveを用いて画像を変換
					warpPerspective(image, outImg, pers_mat, outImg.size());
					mask = cv::Mat::zeros(outImg.size(),CV_8UC1);
					pts[0] = dst_sp;
					pts[1] = dst_2p;
					pts[2] = dst_3p;
					pts[3] = dst_ep;
					contour[0] = pts;
//	確認用描画	cv::circle(image, cv::Point(dst_3p.x, dst_3p.y), 2, cv::Scalar(0, 0, 255), 2, CV_AA);
//	確認用描画	cv::circle(image, cv::Point(dst_ep.x, dst_ep.y), 2, cv::Scalar(0, 255, 0), 2, CV_AA);
					cv::drawContours(mask,contour,0,cv::Scalar::all(255),-1);
//確認用最終マスク画像 outImg.copyTo(image, mask);
					cv::blur(mask, mask, cv::Size(9, 9));
					cv::imshow("Last blured mask", mask);
					// ぼかしたマスクで転写部を背景にアルファブレンディング
					for (int y = 0; y < image.rows; y++){
						uchar* p = image.ptr(y);
						uchar* q = outImg.ptr(y);
						uchar* m = mask.ptr(y);
						for (int x = 0; x < image.cols; x++){ //マスクは1チャネル，他は3チャネル
							p[x*3] = q[x*3] * m[x] / 255 + p[x*3] * (255 - m[x]) / 255;
							p[x*3+1] = q[x*3+1] * m[x] / 255 + p[x*3+1] * (255 - m[x]) / 255;
							p[x*3+2] = q[x*3+2] * m[x] / 255 + p[x*3+2] * (255 - m[x]) / 255;
						}
					}
					x1 = dst_ep.x;
					x2 = dst_3p.x;
					y1 = dst_ep.y;
					y2 = dst_3p.y;
				// a*x + b*y + c = (y2-y1)*x + (x1-x2)*y + (x2-x1)*y1 + (y1-y2)*x1 
					a = (float)(y2-y1);
					b = (float)(x1-x2);
					c = (float)((x2-x1)*y1 + (y1-y2)*x1);
					dst_d3 = (float)abs(a*dst_3p0x + b*dst_3p0y + c)/sqrt(a*a+b*b);
					dst_de = (float)abs(a*dst_ep0x + b*dst_ep0y + c)/sqrt(a*a+b*b);
				}
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
				cv::destroyWindow("Result Image");
				cv::destroyWindow("Perspective Image");
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
