#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;

cv::Mat input;			//入力画像
cv::Mat image;			//処理対象画像
int key = -1;
cv::Point startp, src_sp, dst_sp;		//矩形の始点
cv::Point endp, src_ep, dst_ep;			//矩形の終点
int p_count = 0;
int rect_count = 0;		// 0: コピー元の矩形，1: コピー先の矩形，2: 準備完了
cv::Scalar line_color = cv::Scalar(0, 250, 0);  // 線の色

void onMouseClicks(int event, int x, int y, int flag, void* param) {
	using namespace cv;
	Mat* mat = static_cast<Mat*>(param);	// Mat画像のポインタ受け取り
	Mat c = (*mat).clone(); //元画像をクローン
	//画像の表示
	switch (event)
	{
	case cv::MouseEventTypes::EVENT_LBUTTONDOWN:	//左ボタンが押されたら
		if (rect_count == 2){
			rect_count = 0;
			image = input.clone();
		}
		if (p_count == 0){
			cout << "始点：(" << x << " , " << y << ")" << endl;
			if (rect_count == 0){
				line_color = cv::Scalar(0, 250, 0);  // 線の色
			} else {
				line_color = cv::Scalar(0, 0, 250);  // 線の色
			}
			startp.x = x;
			startp.y = y;
			p_count++;
			break;
		}
		else if (p_count == 1){
			Mat* mat = static_cast<Mat*>(param);
			cout << "終点：(" << x << " , " << y << ")" << endl;
			endp.x = x;
			endp.y = y;
			rectangle(*mat, startp, endp, line_color, 1); //矩形描画
			imshow("mouse input window", *mat);
			p_count++;
			break;
		}

	case cv::MouseEventTypes::EVENT_MOUSEMOVE:
		if (p_count == 1){
			Mat* mat = static_cast<Mat*>(param);
			Mat c = (*mat).clone(); //元画像をクローン
			rectangle(c, startp, Point(x, y), line_color, 1); //矩形描画
			imshow("mouse input window", c);
		}
		break;

	case cv::MouseEventTypes::EVENT_LBUTTONUP:
		if (p_count == 2){
			if (rect_count == 0){
				src_sp = startp;
				src_ep = endp;
				rect_count = 1;
			} else {
				dst_sp = startp;
				dst_ep = endp;
				rect_count = 2;
			}
			p_count = 0;
		}
	}
}

// マウスクリックした4点を入力画像と同サイズの画像の4隅に射影変換する
void rect2rectCopy(char* fileName){
	input = cv::imread(fileName);
	if (input.empty()){
		cout << "画像読み込み失敗" << endl;
		return;
	}
	//ウィンドウ作成
	cv::namedWindow("mouse input window");

	image = input.clone(); //元画像をクローン

	//マウスイベントが発生した時のコールバック関数を登録
	cv::setMouseCallback("mouse input window", onMouseClicks, (void*)&image);

	cv::imshow("mouse input window", image);

	//「q」が押されたら終了
	while (true){
		if (rect_count == 3){
			image = input.clone();
			p_count = 0;
			rect_count = 0;
			cout << "rect_count = 0" << endl;
		} 
		if (rect_count == 2){
			if (src_sp.x > src_ep.x){
				int temp = src_sp.x;
				src_sp.x = src_ep.x;
				src_ep.x = temp;
			}
			if (src_sp.y > src_ep.y){
				int temp = src_sp.y;
				src_sp.y = src_ep.y;
				src_ep.y = temp;
			}
			if (dst_sp.x > dst_ep.x){
				int temp = dst_sp.x;
				dst_sp.x = dst_ep.x;
				dst_ep.x = temp;
			}
			if (dst_sp.y > dst_ep.y){
				int temp = dst_sp.y;
				dst_sp.y = dst_ep.y;
				dst_ep.y = temp;
			}
		}
		int src_width = abs(src_sp.x - src_ep.x);
		int src_height = abs(src_sp.y - src_ep.y);
		int dst_width = abs(dst_sp.x - dst_ep.x);
		int dst_height = abs(dst_sp.y - dst_ep.y);
		cv::Mat src_roi(input, cv::Rect(src_sp.x, src_sp.y, src_width, src_height));	//領域の切り抜き
		cv::Mat result;
	    switch (cv::waitKey(10)){
            case 'c':
				if (rect_count == 2){
					result = src_roi.clone();
            	    cout << "dst_width = " << dst_width << endl;
                	cout << "dst_height = " << dst_height << endl;
					cv:resize(result, result, cv::Size(dst_width, dst_height));
					cv::imshow("destination image", result);
	                cout << "rect to rect copy" << endl;
					rect_count = 3;
				}
                break;
            case 'C':
				if (rect_count == 2){
					float w_rate = src_width/(float)dst_width;
					float h_rate = src_height/(float)dst_height;
					result = src_roi.clone();
					image = input.clone(); //元画像をクローン
					cv::Mat dst_roi(image, cv::Rect(dst_sp.x, dst_sp.y, dst_width, dst_height));	//領域の切り抜き
					for (int y = 0; y < dst_roi.rows; y++){
						uchar* p = src_roi.ptr(int(y*h_rate));
					 	uchar* q = dst_roi.ptr(y);	// Blueチャネル用
					 	for (int x = 0; x < dst_roi.cols; x++){
							 q[x*3] = p[int(x*w_rate)*3];
							 q[x*3+1] = p[int(x*w_rate)*3+1];
							 q[x*3+2] = p[int(x*w_rate)*3+2];
					 	}
	 				}
	                cout << "rect to rect Copy" << endl;
				}
                break;
            case 'r':
				rect_count = 3;
                break;
            case 'R':
				rect_count = 3;
                break;
            case 'q':
                return;
        }
		cv::imshow("mouse input window", image);
		//画像の表示
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2) rect2rectCopy("sample.jpg");	//default画像
	else rect2rectCopy(argv[1]);
	return 0;
}
