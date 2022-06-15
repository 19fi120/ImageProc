#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
	cv::VideoCapture cap(0);	 //カメラ入力の読み込み
	if (cap.isOpened() == false) //カメラ入力の読み込みに失敗したら終了
	{
		cout << "カメラ入力を読み込めませんでした．" << endl;
		return -1;
	}
	// 分類器の読み込み
	string cascadeName = "./haarcascade_frontalface_alt.xml"; // Haar-like
	cv::CascadeClassifier cascade;
	if (!cascade.load(cascadeName))
	{
		cout << "分類器が見つかりませんでした" << endl;
		return -1;
	}
	vector<cv::Rect> faces; //顔検出領域を格納する配列的なもの
	double scale = 2.0;		// 処理時間短縮のために画像縮小率]
	int facerect = 1;
	int key = -1;
	cout << "0: カメラ0(ノートPC付属カメラ), 1: カメラ1(USB接続のカメラ)" << endl;
	cout << "r : 顔に枠表示，d : 顔枠非表示，U/D：スケールUP/DOWN、q : 終了" << endl;

	cv::Mat gray, smallimg;

	while (true)
	{
		cv::Mat frame;
		cap >> frame;	  //カメラからの画像を取得
		if (frame.empty()) //画像が正しく読み込めたのかを確認
		{
			cout << "カメラ入力のために待機" << endl;
			continue;
		}

		cv::cvtColor(frame, gray, cv::ColorConversionCodes::COLOR_BGR2GRAY); // equalizeHist()のために、グレースケール画像へ変換
		cv::equalizeHist(gray, gray);										 // ヒストグラム均一化 ... やらなくとも検出可能

		// 画像サイズを変更調整し，検出対象のサイズの範囲を変更する
		cv::resize(gray, smallimg, cv::Size(frame.cols / scale, frame.rows / scale));

		// ここまでで下準備終わり

		//分類器によって検出する
		cascade.detectMultiScale(smallimg, faces); // 引数：画像，出力矩形（検出された顔を囲む）

		// 結果の描画
		for (int i = 0; i < faces.size(); i++)
		{ //検出した顔の数だけループ
			// 元の画像サイズに合わせるために、scaleをかける
			int facex = faces[i].x * scale,
				facey = faces[i].y * scale,
				facew = faces[i].width * scale,
				faceh = faces[i].height * scale;

			if (facerect == 1)
			{ // 検出された顔を赤枠で囲む
				cv::rectangle(frame, cv::Point(facex, facey), cv::Point(facex + facew, facey + faceh),
							  cv::Scalar(0, 0, 255), 3, 4);
			}
		}

		cv::imshow("Output", frame); //画像の表示
		switch (cv::waitKey(33))
		{
		case 'r':
			facerect = 1; // 検出顔に赤枠
			cout << "r : 顔枠表示" << endl;
			break;
		case 'd':
			facerect = 0;
			cout << "d : 顔枠非表示" << endl;
			break;
		case 'U':
			if (scale <= 10.0)
			{
				scale *= 1.1;
				cout << "scale = " << scale << endl;
			}
			break;
		case 'D':
			if (scale >= 0.6)
			{
				scale *= 0.90;
				cout << "scale = " << scale << endl;
			}
			break;
		case 'q':
			return 0;
		}
	}
}