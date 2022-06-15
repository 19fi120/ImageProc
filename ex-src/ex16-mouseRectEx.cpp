#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;

int key = -1;
cv::Point startp;		//矩形の始点
cv::Point endp;			//矩形の終点
int p_count = 0;
cv::Scalar line_color = cv::Scalar(0, 250, 0);  // 線の色

void onMouseClicks(int event, int x, int y, int flag, void* param) {
	using namespace cv;
	Mat* mat = static_cast<Mat*>(param);	// Mat画像のポインタ受け取り
	Mat c = mat->clone(); //元画像をクローン
	//画像の表示
	switch (event)
	{
	case cv::MouseEventTypes::EVENT_LBUTTONDOWN: //左ボタンが押されたら
		if (p_count == 0){
			cout << "始点：(" << x << " , " << y << ")" << endl;
			startp.x = x;
			startp.y = y;
			p_count++;
		}
		else if (p_count == 1){
			cout << "終点：(" << x << " , " << y << ")" << endl;
			endp.x = x;
			endp.y = y;
			p_count++;
		}
		break;
	case cv::MouseEventTypes::EVENT_MOUSEMOVE:
		if (p_count == 1){
			rectangle(c, startp, Point(x, y), line_color, 2); //矩形描画
			imshow("mouse input window", c);
		}
		break;

	case cv::MouseEventTypes::EVENT_LBUTTONUP:
		if (p_count == 2){
			bool isChange = false;

			if(startp.x > endp.x){
				int temp = startp.x;
				startp.x = endp.x;
				endp.x = temp;
				isChange = true;
			}
			if(startp.y > endp.y){
				int temp = startp.y;
				startp.y = endp.y;
				endp.y = temp;
				isChange = true;
			}
			if(isChange){
				//枠が消える
				//改善が必要
				cout << "変更後" << endl;
				cout << "始点：(" << x << " , " << y << ")" << endl;
				cout << "終点：(" << x << " , " << y << ")" << endl;
				isChange = false;
			}

			rectangle(c, startp, Point(x, y), Scalar(0, 250, 0), 2); //矩形描画
			imshow("mouse input window", c);
			p_count = 0;
		}
	}
}

// マウスクリックした4点を入力画像と同サイズの画像の4隅に射影変換する
void mouseRect(char* inFileName, char* outFilename){
	cv::Mat image = cv::imread(inFileName);
	if (image.empty()){
		cout << "画像読み込み失敗" << endl;
		return;
	}

	//矩形の初期化
	startp.x = 0;
	startp.y = 0;
	endp.x = image.cols-1;
	endp.y = image.rows-1;

	//Gチャネルの塗りつぶす値
	int green = 0;

	//ウィンドウ作成
	cv::namedWindow("mouse input window");

	//マウスイベントが発生した時のコールバック関数を登録
	cv::setMouseCallback("mouse input window", onMouseClicks, (void*)&image);

	//画像の表示
	cv::imshow("mouse input window", image);

	while (true){
		switch (cv::waitKey(33)){
	        case 's': //save
	            cv::imwrite(outFilename, image);
				cv::imshow("mouse input window", image);
	            cout << "saved in: " << outFilename << endl;
	            break;
	        case 'l': //left
                for (int y = startp.y; y < endp.y; y++){
		            uchar* p = image.ptr(y);
            		for (int x = startp.x + 1; x < endp.x; x++){
    			        p[x*3] = p[startp.x*3];     // B
    			        p[x*3+1] = p[startp.x*3+1];  // G
    			        p[x*3+2] = p[startp.x*3+2]; // R
                    }
	            }
				cv::imshow("mouse input window", image);
	            break;
	        case 'r': //right
				for (int y = startp.y; y < endp.y; y++){
		            uchar* p = image.ptr(y);
            		for (int x = startp.x + 1; x < endp.x; x++){
    			        p[x*3] = p[endp.x*3];     // B
    			        p[x*3+1] = p[endp.x*3+1];  // G
    			        p[x*3+2] = p[endp.x*3+2]; // R
                    }
	            }
				cv::imshow("mouse input window", image);
	            break;
			case 't': //top
				for (int y = startp.y; y < endp.y; y++){
		            uchar* p = image.ptr(y);
            		for (int x = startp.x + 1; x < endp.x; x++){
						uchar* q = image.ptr(startp.y);
    			        p[x*3] = q[x*3];     // B
    			        p[x*3+1] = q[x*3+1];  // G
    			        p[x*3+2] = q[x*3+2]; // R
                    }
	            }
				cv::imshow("mouse input window", image);
	            break;
			case 'b': //botm
				for (int y = startp.y; y < endp.y; y++){
		            uchar* p = image.ptr(y);
            		for (int x = startp.x + 1; x < endp.x; x++){
						uchar* q = image.ptr(endp.y);
    			        p[x*3] = q[x*3];     // B
    			        p[x*3+1] = q[x*3+1];  // G
    			        p[x*3+2] = q[x*3+2]; // R
                    }
	            }
				cv::imshow("mouse input window", image);
	            break;
			case 'h': //
				for (int y = startp.y; y < endp.y; y++){
		            uchar* p = image.ptr(y);
            		for (int x = startp.x + 1; x < endp.x; x++){
    			        p[x*3] = p[startp.x*3] + (p[endp.x*3]-p[startp.x*3]) * (x-startp.x*3) / (endp.x*3-startp.x*3);  // B
    			        p[x*3+1] = p[startp.x*3+1] + (p[endp.x*3+1]-p[startp.x*3+1]) * (x-startp.x*3+1) / (endp.x*3+1-startp.x*3+1);  // G
    			        p[x*3+2] = p[startp.x*3+2] + (p[endp.x*3+2]-p[startp.x*3+2]) * (x-startp.x*3+2) / (endp.x*3+2-startp.x*3+2);  // R
                    }
	            }
				cv::imshow("mouse input window", image);
	            break;
			case 'v': //
				for (int y = startp.y; y < endp.y; y++){
		            uchar* p = image.ptr(y);
					uchar* qs = image.ptr(startp.y);
					uchar* qe = image.ptr(endp.y);
            		for (int x = startp.x + 1; x < endp.x; x++){
    			        p[x*3] = qs[x*3] + (qe[x*3]-qs[x*3]) * (y-startp.y) / (endp.x*3-startp.x*3);  // B
    			        p[x*3+1] = qs[x*3+1] + (qe[x*3+1]-qs[x*3+1]) * (y-startp.y*3+1) / (endp.x*3+1-startp.x*3+1);  // G
    			        p[x*3+2] = qs[x*3+2] + (qe[x*3+2]-qs[x*3+2]) * (y-startp.y*3+2) / (endp.x*3+2-startp.x*3+2);  // R
                    }
	            }
				cv::imshow("mouse input window", image);
	            break;

				
	        case 'q':
	            return;
	    }
    }
}

int main(int argc, char* argv[])
{
	if (argc < 2) mouseRect("sample.jpg", "result.jpg");	//default画像
	else if (argc < 3) mouseRect(argv[1], "result.jpg");
    else mouseRect(argv[1], argv[2]);
	return 0;
}
