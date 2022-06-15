// c: ヒストグラム表示

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
cv::Mat outImg2;
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

//棒グラフの表示
void barGraph(cv::Mat hist, vector<int>& histVector)
{
	// グラフ幅
	int width = hist.cols;
	// グラフ高さ
	int height = hist.rows;
	// 要素数
	int num = histVector.size();
	cout << "size of histVector: " << num << endl;
	// 最大要素
	int max = histVector[0];
	for(int i = 1; i < num; i++){
		if (histVector[i] > max) max = histVector[i];
	}
	// または int max = *max_element( histVector.begin(), histVector.end() );
	// 棒グラフ配置設計
	double dw = (double)width/(num*2+1);	// 棒の幅と間隔を同じに
	cout << "size of bar width: " << dw << endl;
	double dh = (double)height/max;				// 度数あたりの高さ画素数
	cout << "height unit: " << dh << endl;
	
	hist = cv::Scalar::all(255);	// 全面白色で初期化
	// 各棒の描画
	for(int i = 0; i < num; i++){
		double x = dw*(2*i+1);					// 棒グラフ内描画用x座標
		double y = height - histVector[i]*dh;	// 棒グラフ内描画用y座標
		cout << "i, count: " << i << ", " << histVector[i] << endl;
		cv::rectangle(hist, cv::Rect2d(x,y,dw,histVector[i]*dh), cv::Scalar(255, 0, 0), cv::FILLED);
	}
	return;
}

void calcHistgram(cv::Mat hist, vector<int>& histVector)
{
    // グラフ幅
	int cols = hist.cols;
	// グラフ高さ
	int rows = hist.rows;
    // 要素数
	int num = histVector.size();
	//cout << "size of histVector: " << num << endl;
    for(int i = 0; i < rows; i++){
        uchar *p = hist.ptr(i);
        for(int j = 0; j < cols; j++){
            histVector[p[j] * num / 256]++;
        }
    }
}

//ヒストグラムを描画
void colorSim(char **args)
{
    Mat hsv;
    Mat matArray[3];
    Mat hsv2;
    Mat matArray2[3];
	input = cv::imread(args[1]);
	if (input.empty()){
		cout << "画像読み込み失敗" << endl;
		return;
	}
    int binWidth = atoi(args[2]);
    int width = atoi(args[3]);
    int height = atoi(args[4]);
    Mat hist;
    Mat hist2;

	//マウス入力用画像
	c = input.clone();
	imageTmp = input.clone();
	imageLine = input.clone();

	//ウィンドウ作成
	cv::namedWindow("mouse input window");

	//マウスイベントが発生した時のコールバック関数を登録
	cv::setMouseCallback("mouse input window", onMouseClicks, (void*)&imageLine);

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
                //棒グラフ幅(default=6)
                //hueの取り出し
                vector<int> histVector((int)(180/binWidth));    //度数表宣言
                vector<int> histVector2((int)(180/binWidth));   //度数表宣言
                cvtColor(outImg, hsv, ColorConversionCodes::COLOR_BGR2HSV); //RGB -> HSV
                split(hsv, matArray);
                calcHistgram(matArray[0], histVector);  //hueの度数表作成
                //棒グラフウィンドウ
                hist.create(cv::Size(width, height), CV_8UC3);	// カラー画像生成
                hist = Scalar::all(255);
                barGraph(hist, histVector);
                cv::imshow("Input", input);
                cv::imshow("Hue", matArray[0]);
                cv::imshow("Histgram1", hist);

				//hueの取り出し
                cvtColor(outImg2, hsv2, ColorConversionCodes::COLOR_BGR2HSV); //RGB -> HSV
                split(hsv2, matArray2);
                calcHistgram(matArray2[0], histVector2);  //hueの度数表作成
                //棒グラフウィンドウ
                hist2.create(cv::Size(width, height), CV_8UC3);	// カラー画像生成
                hist = Scalar::all(255);
                barGraph(hist2, histVector2);
                cv::imshow("Hue", matArray2[0]);
                cv::imshow("Histgram2", hist2);
	            cv::waitKey();						    //入力があるまで待機
            }
			break;
	        case 'q':
	            return;
	    }
    }
}

int main(int argc, char* argv[])
{
                                //入力画像、bin幅、width,height
	char *args[5] = {argv[0], "../image/imageA.jpg", "10", "400", "300"};	//default画像
	if (argc >= 2)
		args[1] = argv[1]; // 画像
	if (argc >= 3)
		args[2] = argv[2]; // bin
    if (argc >= 4)
		args[3] = argv[3]; // width
	if (argc >= 5)
		args[4] = argv[4]; // height

	colorSim(args);
	return 0;
}
